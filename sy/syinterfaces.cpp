// syinterfaces.cpp
//
// Network interface information
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of version 2.1 of the GNU Lesser General Public
//    License as published by the Free Software Foundation;
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, 
//    Boston, MA  02111-1307  USA
//

#ifdef WIN32
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iphlpapi.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#endif  // WIN32
#ifdef OSX
#include <ifaddrs.h>
#endif  // OSX

#include "syinterfaces.h"
#include "syrouting.h"
#include "sysyslog.h"

SyInterfaces::SyInterfaces()
{
}


int SyInterfaces::quantity() const
{
  return iface_names.size();
}


QString SyInterfaces::name(int n) const
{
  return iface_names.at(n);
}


uint64_t SyInterfaces::macAddress(int n)
{
  return iface_mac_addresses.at(n);
}


QHostAddress SyInterfaces::ipv4Address(int n) const
{
  return iface_ipv4_addresses.at(n);
}


QHostAddress SyInterfaces::ipv4Netmask(int n) const
{
  return iface_ipv4_netmasks.at(n);
}


bool SyInterfaces::update()
{
  int sock;

  if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    SySyslog(LOG_ERR,
      SyRouting::socketErrorString("unable to create socket"));
    return false;
  }
  iface_names.clear();
  iface_mac_addresses.clear();
  iface_ipv4_addresses.clear();
  iface_ipv4_netmasks.clear();

#ifdef LINUX
  struct ifreq ifr;
  int index=0;

  memset(&ifr,0,sizeof(ifr));
  index=1;
  ifr.ifr_ifindex=index;
  while(ioctl(sock,SIOCGIFNAME,&ifr)==0) {
    iface_names.push_back(ifr.ifr_name);
    iface_mac_addresses.push_back(0);
    iface_ipv4_addresses.push_back(QHostAddress());
    iface_ipv4_netmasks.push_back(QHostAddress());
    if(ioctl(sock,SIOCGIFHWADDR,&ifr)==0) {
      iface_mac_addresses.back()=
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[0])<<40)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[1])<<32)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[2])<<24)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[3])<<16)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[4])<<8)+
	(0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[5]);
      if(iface_mac_addresses.back()!=0) {
	if(ioctl(sock,SIOCGIFADDR,&ifr)==0) {
	  struct sockaddr_in sa=*(sockaddr_in *)(&ifr.ifr_addr);
	  iface_ipv4_addresses.back().setAddress(ntohl(sa.sin_addr.s_addr));
	}
	if(ioctl(sock,SIOCGIFNETMASK,&ifr)==0) {
	  struct sockaddr_in sa=*(sockaddr_in *)(&ifr.ifr_netmask);
	  iface_ipv4_netmasks.back().setAddress(ntohl(sa.sin_addr.s_addr));
	}
      }
    }
    ifr.ifr_ifindex=++index;
  }
  close(sock);
#endif  // LINUX
  return true;
}


QString SyInterfaces::macString(uint64_t maddr)
{
  return QString().sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
			   (unsigned)(0xFF&(maddr>>40)),
			   (unsigned)(0xFF&(maddr>>32)),
			   (unsigned)(0xFF&(maddr>>24)),
			   (unsigned)(0xFF&(maddr>>16)),
			   (unsigned)(0xFF&(maddr>>8)),
			   (unsigned)(0xFF&maddr));
}
