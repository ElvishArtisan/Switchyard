// routing.cpp
//
// AoIP stream routing configuration
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include <QtCore/QDir>
#include <QtCore/QStringList>

#include "profile.h"
#include "routing.h"

Routing::Routing(unsigned d_slots,unsigned s_slots)
{
  QDir dir(SWITCHYARD_CACHE_DIR);
  if(!dir.exists(SWITCHYARD_CACHE_DIR)) {
    dir.mkpath(SWITCHYARD_CACHE_DIR);
  }

  if((d_slots>=SWITCHYARD_MAX_SLOTS)||(s_slots>=SWITCHYARD_MAX_SLOTS)) {
    syslog(LOG_ERR,"maximum slot count exceeded");
    exit(256);
  }
  dst_slots=d_slots;
  src_slots=s_slots;
  LoadInterfaces();
  load();
}


unsigned Routing::dstSlots() const
{
  return dst_slots;
}


unsigned Routing::srcSlots() const
{
  return src_slots;
}


QString Routing::hostName() const
{
#ifdef OSX
  char hostname[sysconf(_SC_HOST_NAME_MAX)];
    gethostname(hostname,sysconf(_SC_HOST_NAME_MAX));
#else
  char hostname[HOST_NAME_MAX];
  gethostname(hostname,HOST_NAME_MAX);
#endif  // OSX
  return QString(hostname).split(".")[0];
}


QHostAddress Routing::nicAddress() const
{
  return QHostAddress(ntohl(nic_addr));
}


void Routing::setNicAddress(const QHostAddress &addr)
{
  nic_addr=htonl(addr.toIPv4Address());
}


QHostAddress Routing::nicNetmask() const
{
  return QHostAddress(ntohl(nic_mask));
}


void Routing::setNicNetmask(const QHostAddress &addr)
{
  nic_mask=htonl(addr.toIPv4Address());
}


QHostAddress Routing::clkAddress() const
{
  return QHostAddress(ntohl(clk_addr));
}


void Routing::setClkAddress(const QHostAddress &addr)
{
  clk_addr=htonl(addr.toIPv4Address());
}


int Routing::srcNumber(int slot) const
{
  return 0xFFFF&srcAddress(slot).toIPv4Address();
}


QHostAddress Routing::srcAddress(int slot) const
{
  return QHostAddress(ntohl(src_addr[slot]));
}


void Routing::setSrcAddress(int slot,const QHostAddress &addr)
{
  src_addr[slot]=htonl(addr.toIPv4Address());
}


void Routing::setSrcAddress(int slot,const QString &addr)
{
  QHostAddress a(addr);
  setSrcAddress(slot,a);
}


QString Routing::srcName(int slot) const
{
  return sy_src_names[slot];
}


void Routing::setSrcName(int slot,const QString &str)
{
  sy_src_names[slot]=str;
}


bool Routing::srcEnabled(int slot) const
{
  return src_enabled[slot];
}


void Routing::setSrcEnabled(int slot,bool state)
{
  src_enabled[slot]=state;
}


int Routing::srcMeterLevel(int slot,int chan) const
{
  return (int)(2000.0*log10f(src_meter[slot][chan]));
}


int Routing::activeSources() const
{
  int ret=0;

  for(unsigned i=0;i<srcSlots();i++) {
    if((!srcAddress(i).isNull())&&srcEnabled(i)) {
      ret++;
    }
  }

  return ret;
}


QHostAddress Routing::dstAddress(int slot) const
{
  return QHostAddress(ntohl(dst_addr[slot]));
}


void Routing::setDstAddress(int slot,const QHostAddress &addr)
{
  dst_addr[slot]=htonl(addr.toIPv4Address());
}


void Routing::setDstAddress(int slot,const QString &addr)
{
  QHostAddress a(addr);
  setDstAddress(slot,a);
}


QString Routing::dstName(int slot) const
{
  return sy_dst_names[slot];
}


void Routing::setDstName(int slot,const QString &str)
{
  sy_dst_names[slot]=str;
}


int Routing::dstMeterLevel(int slot,int chan) const
{
  return (int)(2000.0*log10f(dst_meter[slot][chan]));
}


unsigned Routing::nicQuantity() const
{
  return sy_nic_devices.size();
}


QHostAddress Routing::nicAddress(unsigned n)
{
  return sy_nic_addresses[n];
}


QHostAddress Routing::nicNetmask(unsigned n)
{
  return sy_nic_netmasks[n];
}


QString Routing::nicDevice(unsigned n)
{
  return sy_nic_devices[n];
}


void Routing::subscribe(const QHostAddress &addr)
{
  struct ip_mreqn mreq;

  memset(&mreq,0,sizeof(mreq));
  mreq.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  mreq.imr_address.s_addr=nic_addr;
  mreq.imr_ifindex=0;
#ifdef OSX
  setsockopt(sy_subscription_socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,
	     &mreq,sizeof(mreq));
#else
  setsockopt(sy_subscription_socket,SOL_IP,IP_ADD_MEMBERSHIP,
	     &mreq,sizeof(mreq));
#endif
}


void Routing::unsubscribe(const QHostAddress &addr)
{
  struct ip_mreqn mreq;

  memset(&mreq,0,sizeof(mreq));
  mreq.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  mreq.imr_address.s_addr=nic_addr;
  mreq.imr_ifindex=0;
#ifdef OSX
  setsockopt(sy_subscription_socket,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq));
#else
  setsockopt(sy_subscription_socket,SOL_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq));
#endif  // OSX
}


int Routing::subscriptionSocket() const
{
  return sy_subscription_socket;
}


void Routing::load()
{
  QString section;
  Profile *p=new Profile();
  p->setSource(SWITCHYARD_ROUTING_FILE);
  QHostAddress default_nic;
  if(nicQuantity()>0) {
    default_nic=nicAddress(0);
  }

  setNicAddress(p->addressValue("Global","NicAddress",default_nic));
  for(unsigned i=0;i<srcSlots();i++) {
    section=QString().sprintf("Slot%u",i+1);
    setSrcAddress(i,p->addressValue(section,"SourceAddress",""));
    setSrcName(i,p->stringValue(section,"SourceName",QString().sprintf("Source %u",i+1)));
    setSrcEnabled(i,p->intValue(section,"SourceEnabled"));
  }
  for(unsigned i=0;i<dstSlots();i++) {
    setDstAddress(i,p->addressValue(section,"DestinationAddress",""));
    setDstName(i,p->stringValue(section,"DestinationName",QString().sprintf("Destination %u",i+1)));
  }

  delete p;
}


void Routing::save() const
{
  FILE *f=NULL;
  QString tempfile=QString(SWITCHYARD_ROUTING_FILE)+"-temp";

  if((f=fopen(tempfile.toAscii(),"w"))==NULL) {
    syslog(LOG_WARNING,"unable to save routing data [%s]",strerror(errno));
    return;
  }

  fprintf(f,"[Global]\n");
  fprintf(f,"NicAddress=%s\n",(const char *)nicAddress().toString().toAscii());
  fprintf(f,"\n");
  for(int i=0;i<SWITCHYARD_MAX_SLOTS;i++) {
    fprintf(f,"[Slot%u]\n",i+1);
    if(i<(int)srcSlots()) {
      fprintf(f,"SourceAddress=%s\n",
	      (const char *)srcAddress(i).toString().toAscii());
      fprintf(f,"SourceName=%s\n",(const char *)srcName(i).toAscii());
      fprintf(f,"SourceEnabled=%d\n",srcEnabled(i));
    }
    if(i<(int)dstSlots()) {
      fprintf(f,"DestinationAddress=%s\n",
	      (const char *)dstAddress(i).toString().toAscii());
      fprintf(f,"DestinationName=%s\n",(const char *)dstName(i).toAscii());
    }
    fprintf(f,"\n");
  }

  fclose(f);
  rename(tempfile.toAscii(),SWITCHYARD_ROUTING_FILE);
}


QString Routing::dumpAddress(uint32_t addr)
{
  QHostAddress a(ntohl(addr));
  return a.toString();
}


void Routing::LoadInterfaces()
{
  if((sy_subscription_socket=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    syslog(LOG_ERR,"unable to create RTP subscription socket [%s]",
	   strerror(errno));
    exit(256);
  }

#ifdef OSX
  struct ifaddrs *ifap=NULL;
  struct ifaddrs *ifa=NULL;
  struct sockaddr_in *sa_in=NULL;

  if(getifaddrs(&ifap)<0) {
    syslog(LOG_ERR,"unable to get interface information [%s]",strerror(errno));
    exit(256);
  }
  ifa=ifap;
  do {
    switch(ifa->ifa_addr->sa_family) {
    case AF_INET:
      sa_in=(struct sockaddr_in *)ifa->ifa_addr;
      if((QString(ifa->ifa_name).left(2)=="en")&&(sa_in->sin_addr.s_addr!=0)) {
	sy_nic_addresses.push_back(QHostAddress(ntohl(sa_in->sin_addr.s_addr)));
	sa_in=(struct sockaddr_in *)ifa->ifa_netmask;
	sy_nic_netmasks.push_back(QHostAddress(ntohl(sa_in->sin_addr.s_addr)));
	//
	// FIXME: How do we read MAC addresses on OS X?
	//
	sy_nic_devices.push_back(QString(ifa->ifa_name)+": 00:00:00:00:00:00");
      }
      if(QString(ifa->ifa_name)=="en0") {
	nic_addr=htonl(sy_nic_addresses.back().toIPv4Address());
	nic_mask=htonl(sy_nic_netmasks.back().toIPv4Address());
      }
      break;
    }
  } while((ifa=ifa->ifa_next)!=NULL);
  freeifaddrs(ifap);
  /*
  for(unsigned i=0;i<sy_nic_addresses.size();i++) {
    printf("%s: %s  %s\n",(const char *)sy_nic_devices[i].toAscii(),
	   (const char *)sy_nic_addresses[i].toString().toAscii(),
	   (const char *)sy_nic_netmasks[i].toString().toAscii());
  }
  */
#else
  struct ifreq ifr;
  int index=0;
  uint64_t mac;

  memset(&ifr,0,sizeof(ifr));
  index=1;
  ifr.ifr_ifindex=index;
  while(ioctl(sy_subscription_socket,SIOCGIFNAME,&ifr)==0) {
    if(ioctl(sy_subscription_socket,SIOCGIFHWADDR,&ifr)==0) {
      mac=
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[0])<<40)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[1])<<32)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[2])<<24)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[3])<<16)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[4])<<8)+
	(0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[5]);
      if(mac!=0) {
	sy_nic_devices.
	  push_back(QString().
		    sprintf("%s: %02X:%02X:%02X:%02X:%02X:%02X",
			    ifr.ifr_name,
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[0],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[1],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[2],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[3],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[4],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[5]));
	sy_nic_addresses.push_back(QHostAddress());
	sy_nic_netmasks.push_back(QHostAddress());
	if(ioctl(sy_subscription_socket,SIOCGIFADDR,&ifr)==0) {
	  struct sockaddr_in sa=*(sockaddr_in *)(&ifr.ifr_addr);
	  sy_nic_addresses.back().setAddress(ntohl(sa.sin_addr.s_addr));
	}
	if(ioctl(sy_subscription_socket,SIOCGIFNETMASK,&ifr)==0) {
	  struct sockaddr_in sa=*(sockaddr_in *)(&ifr.ifr_netmask);
	  sy_nic_netmasks.back().setAddress(ntohl(sa.sin_addr.s_addr));
	}
	if(strcmp(ifr.ifr_name,"eth0")==0) {
	  nic_addr=htonl(sy_nic_addresses.back().toIPv4Address());
	  nic_mask=htonl(sy_nic_netmasks.back().toIPv4Address());
	}
      }
    }
    ifr.ifr_ifindex=++index;
  }
#endif  // OSX
}
