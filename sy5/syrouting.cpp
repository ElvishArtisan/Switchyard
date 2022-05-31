// syrouting.cpp
//
// AoIP stream routing configuration
//
// (C) 2014-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef WIN32
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iphlpapi.h>
#else
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#endif  // WIN32
#ifdef OSX
#include <ifaddrs.h>
#endif  // OSX

#include <QDir>
#include <QSettings>
#include <QStringList>

#include "sysyslog.h"
#include "syprofile.h"
#include "syrouting.h"

SyRouting::SyRouting(unsigned d_slots,unsigned s_slots,unsigned gpis,unsigned gpos)
{
  //
  // Initialize Networking
  //
#ifdef WIN32
  WSADATA data;
  if(WSAStartup(2,&data)!=0) {
    SySyslog(LOG_ERR,"Winsock initialialization failed");
    exit(256);
  }
#endif  // WIN32

  //
  // Create Database Directory
  //
  QDir dir(SWITCHYARD_CACHE_DIR);
  if(!dir.exists(SWITCHYARD_CACHE_DIR)) {
    dir.mkpath(SWITCHYARD_CACHE_DIR);
  }

  //
  // Initialize GPIO Subsystem
  //
  for(unsigned i=0;i<(gpis*SWITCHYARD_GPIO_BUNDLE_SIZE);i++) {
    sy_gpi_states.push_back(0);
  }
  for(unsigned i=0;i<(gpos*SWITCHYARD_GPIO_BUNDLE_SIZE);i++) {
    sy_gpo_states.push_back(0);
  }

  if((d_slots>SWITCHYARD_MAX_SLOTS)||(s_slots>SWITCHYARD_MAX_SLOTS)) {
    SySyslog(LOG_ERR,"maximum slot count exceeded");
    exit(256);
  }
  dst_slots=d_slots;
  src_slots=s_slots;
  LoadInterfaces();
  load();
}


unsigned SyRouting::dstSlots() const
{
  return dst_slots;
}


unsigned SyRouting::srcSlots() const
{
  return src_slots;
}


unsigned SyRouting::gpis() const
{
  return sy_gpi_states.size()/SWITCHYARD_GPIO_BUNDLE_SIZE;
}


unsigned SyRouting::gpos() const
{
  return sy_gpo_states.size()/SWITCHYARD_GPIO_BUNDLE_SIZE;
}


QString SyRouting::hostName() const
{
#ifdef OSX
  char hostname[sysconf(_SC_HOST_NAME_MAX)];
    gethostname(hostname,sysconf(_SC_HOST_NAME_MAX));
#endif  // OSX
#ifdef LINUX
  char hostname[HOST_NAME_MAX];
  gethostname(hostname,HOST_NAME_MAX);
#endif  // LINUX
#ifdef WIN32
  char hostname[256];
  gethostname(hostname,256);
#endif  // WIN32
  return QString(hostname).split(".")[0];
}


QHostAddress SyRouting::nicAddress() const
{
  return QHostAddress(ntohl(nic_addr));
}


void SyRouting::setNicAddress(const QHostAddress &addr)
{
  nic_addr=htonl(addr.toIPv4Address());
}


QHostAddress SyRouting::nicNetmask() const
{
  return QHostAddress(ntohl(nic_mask));
}


void SyRouting::setNicNetmask(const QHostAddress &addr)
{
  nic_mask=htonl(addr.toIPv4Address());
}


QHostAddress SyRouting::clkAddress() const
{
  return QHostAddress(ntohl(clk_addr));
}


void SyRouting::setClkAddress(const QHostAddress &addr)
{
  clk_addr=htonl(addr.toIPv4Address());
}


int SyRouting::srcNumber(int slot) const
{
  return 0xFFFF&srcAddress(slot).toIPv4Address();
}


QHostAddress SyRouting::srcAddress(int slot) const
{
  return QHostAddress(ntohl(src_addr[slot]));
}


void SyRouting::setSrcAddress(int slot,const QHostAddress &addr)
{
  src_addr[slot]=htonl(addr.toIPv4Address());
}


void SyRouting::setSrcAddress(int slot,const QString &addr)
{
  QHostAddress a(addr);
  setSrcAddress(slot,a);
}


QString SyRouting::srcName(int slot) const
{
  return sy_src_names[slot];
}


void SyRouting::setSrcName(int slot,const QString &str)
{
  sy_src_names[slot]=str;
}


bool SyRouting::srcEnabled(int slot) const
{
  return src_enabled[slot];
}


void SyRouting::setSrcEnabled(int slot,bool state)
{
  src_enabled[slot]=state;
}


int SyRouting::srcMeterLevel(int slot,int chan) const
{
  return (int)(2000.0*log10f(src_meter[slot][chan]));
}


int SyRouting::activeSources() const
{
  int ret=0;

  for(unsigned i=0;i<srcSlots();i++) {
    if((!srcAddress(i).isNull())&&srcEnabled(i)) {
      ret++;
    }
  }

  return ret;
}


QHostAddress SyRouting::dstAddress(int slot) const
{
  return QHostAddress(ntohl(dst_addr[slot]));
}


void SyRouting::setDstAddress(int slot,const QHostAddress &addr)
{
  dst_addr[slot]=htonl(addr.toIPv4Address());
}


void SyRouting::setDstAddress(int slot,const QString &addr)
{
  QHostAddress a(addr);
  setDstAddress(slot,a);
}


QString SyRouting::dstName(int slot) const
{
  return sy_dst_names[slot];
}


void SyRouting::setDstName(int slot,const QString &str)
{
  sy_dst_names[slot]=str;
}


int SyRouting::dstMeterLevel(int slot,int chan) const
{
  return (int)(2000.0*log10f(dst_meter[slot][chan]));
}


bool SyRouting::gpiStateBySlot(int slot,int line) const
{
  return sy_gpi_states[slot*SWITCHYARD_GPIO_BUNDLE_SIZE+line];
}

void SyRouting::setGpi(int srcnum,int line,bool state,bool pulse)
{
  for(unsigned i=0;i<gpis();i++) {
    if(gpoAddress(i)==SyRouting::streamAddress(SyRouting::Stereo,srcnum)) {
      sy_gpi_states[i+line]=state;
    }
  }
}


void SyRouting::setGpiBySlot(int slot,int line,bool state,bool pulse)
{
  sy_gpi_states[slot+line]=state;
}


bool SyRouting::gpoStateBySlot(int slot,int line) const
{
  return sy_gpo_states[slot*SWITCHYARD_GPIO_BUNDLE_SIZE+line];
}


void SyRouting::setGpo(int srcnum,int line,bool state,bool pulse)
{
  for(unsigned i=0;i<gpos();i++) {
    if(gpoAddress(i)==SyRouting::streamAddress(SyRouting::Stereo,srcnum)) {
      sy_gpo_states[i+line]=state;
    }
  }
}


void SyRouting::setGpoBySlot(int slot,int line,bool state,bool pulse)
{
  sy_gpo_states[slot+line]=state;
}


SyRouting::GpoMode SyRouting::gpoMode(int slot) const
{
  return sy_gpo_modes[slot];
}


void SyRouting::setGpoMode(int slot,SyRouting::GpoMode mode)
{
  sy_gpo_modes[slot]=mode;
}


QHostAddress SyRouting::gpoAddress(int slot) const
{
  return QHostAddress(gpo_addr[slot]);
}


void SyRouting::setGpoAddress(int slot,const QHostAddress &addr)
{
  gpo_addr[slot]=addr.toIPv4Address();
}


void SyRouting::setGpoAddress(int slot,const QString &addr)
{
  gpo_addr[slot]=QHostAddress(addr).toIPv4Address();
}


int SyRouting::gpoSnakeSlot(int slot) const
{
  return sy_gpo_snake_slots[slot];
}


void SyRouting::setGpoSnakeSlot(int slot,int snake_slot)
{
  sy_gpo_snake_slots[slot]=snake_slot;
}


QString SyRouting::gpoName(int slot) const
{
  return sy_gpo_names[slot];
}


void SyRouting::setGpoName(int slot,const QString &str)
{
  sy_gpo_names[slot]=str;
}


unsigned SyRouting::nicQuantity() const
{
  return sy_nic_devices.size();
}


QHostAddress SyRouting::nicAddress(unsigned n)
{
  return sy_nic_addresses[n];
}


QHostAddress SyRouting::nicNetmask(unsigned n)
{
  return sy_nic_netmasks[n];
}


QString SyRouting::nicDevice(unsigned n)
{
  return sy_nic_devices[n];
}


void SyRouting::subscribe(const QHostAddress &addr)
{
#ifdef WIN32
  struct ip_mreq_source imr;

  memset(&imr,0,sizeof(imr));
  imr.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  imr.imr_interface.s_addr=nic_addr;
  setsockopt(sy_subscription_socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,
	     (char *)&imr,sizeof(imr));
#else
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
#endif  // OSX

#endif // WIN32
}


void SyRouting::unsubscribe(const QHostAddress &addr)
{
#ifdef WIN32
  struct ip_mreq_source imr;

  memset(&imr,0,sizeof(imr));
  imr.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  imr.imr_interface.s_addr=nic_addr;
  setsockopt(sy_subscription_socket,IPPROTO_IP,IP_DROP_MEMBERSHIP,
	     (char *)&imr,sizeof(imr));
#else
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

#endif  // WIN32
}


int SyRouting::subscriptionSocket() const
{
  return sy_subscription_socket;
}


int SyRouting::rtpSendSocket() const
{
  return sy_rtp_send_socket;
}


void SyRouting::load()
{
#ifdef WIN32
  QSettings *s= new QSettings(QSettings::SystemScope,
			      SWITCHYARD_SETTINGS_ORGANIZATION,
			      SWITCHYARD_SETTINGS_APPLICATION);
  setNicAddress(QHostAddress(s->value("NicAddress").toString()));
  for(unsigned i=0;i<srcSlots();i++) {
    QString key=QString::asprintf("Slot%u",i+1);
    setSrcAddress(i,s->value(key+"/SourceAddress").toString());
    setSrcName(i,s->value(key+"/SourceName").toString());
    setSrcEnabled(i,s->value(key+"/SourceEnabled").toUInt());
  }
  for(unsigned i=0;i<dstSlots();i++) {
    QString key=QString::asprintf("Slot%u",i+1);
    setDstAddress(i,s->value(key+"/DestinationAddress").toString());
    setDstName(i,s->value(key+"/DestinationName").toString());
  }
  for(unsigned i=0;i<gpos();i++) {
    QString key=QString::asprintf("Slot%u",i+1);
    setGpoMode(i,(SyRouting::GpoMode)s->value(key+"/GpoMode").toInt());
    setGpoAddress(i,s->value(key+"/GpoAddress").toString());
    setGpoSnakeSlot(i,s->value(key+"/GpoSnakeSlot").toInt());
    setGpoName(i,s->value(key+"/GpoName").toString());
    if(gpoName(i).isEmpty()) {
      setGpoName(i,QString::asprintf("GPIO %d",i+1));
    }
  }
  delete s;
#else
  QString section;
  SyProfile *p=new SyProfile();
  p->setSource(SWITCHYARD_ROUTING_FILE);
  QHostAddress default_nic;
  if(nicQuantity()>0) {
    default_nic=nicAddress(0);
  }

  setNicAddress(p->addressValue("Global","NicAddress",default_nic));
  for(unsigned i=0;i<srcSlots();i++) {
    section=QString::asprintf("Slot%u",i+1);
    setSrcAddress(i,p->addressValue(section,"SourceAddress",""));
    setSrcName(i,p->stringValue(section,"SourceName",QString::asprintf("Source %u",i+1)));
    setSrcEnabled(i,p->intValue(section,"SourceEnabled"));
  }
  for(unsigned i=0;i<dstSlots();i++) {
    section=QString::asprintf("Slot%u",i+1);
    setDstAddress(i,p->addressValue(section,"DestinationAddress",""));
    setDstName(i,p->stringValue(section,"DestinationName",QString::asprintf("Destination %u",i+1)));
  }
  for(unsigned i=0;i<gpos();i++) {
    section=QString::asprintf("Slot%u",i+1);
    setGpoMode(i,(SyRouting::GpoMode)p->intValue(section,"GpoMode"));
    setGpoAddress(i,p->addressValue(section,"GpoAddress",""));
    setGpoSnakeSlot(i,p->intValue(section,"GpoSnakeSlot"));
    setGpoName(i,p->stringValue(section,"GpoName",gpoName(i)));
    if(gpoName(i).isEmpty()) {
      setGpoName(i,QString::asprintf("GPIO %d",i+1));
    }
  }
  delete p;
#endif  // WIN32
}


void SyRouting::save() const
{
#ifdef WIN32
  QSettings *s= new QSettings(QSettings::SystemScope,
			      SWITCHYARD_SETTINGS_ORGANIZATION,
			      SWITCHYARD_SETTINGS_APPLICATION);
  s->setValue("NicAddress",nicAddress().toString());
  for(int i=0;i<SWITCHYARD_MAX_SLOTS;i++) {
    QString key=QString::asprintf("Slot%u",i+1);
    if(i<(int)srcSlots()) {
      s->setValue(key+"/SourceAddress",srcAddress(i).toString());
      s->setValue(key+"/SourceName",srcName(i));
      s->setValue(key+"/SourceEnabled",srcEnabled(i));
    }
    if(i<(int)dstSlots()) {
      s->setValue(key+"/DestinationAddress",dstAddress(i).toString());
      s->setValue(key+"/DestinationName",dstName(i));
    }
    if(i<(int)gpos()) {
      s->setValue(key+"/GpoMode",(int)gpoMode(i));
      s->setValue(key+"/GpoAddress",gpoAddress(i).toString());
      s->setValue(key+"/GpoSnakeSlot",gpoSnakeSlot(i));
      s->setValue(key+"/GpoName",gpoName(i));
    }
  }
  delete s;
#else
  FILE *f=NULL;
  QString tempfile=QString(SWITCHYARD_ROUTING_FILE)+"-temp";

  if((f=fopen(tempfile.toUtf8(),"w"))==NULL) {
    SySyslog(LOG_WARNING,QString::asprintf("unable to save routing data [%s]",
					   strerror(errno)));
    return;
  }

  fprintf(f,"[Global]\n");
  fprintf(f,"NicAddress=%s\n",nicAddress().toString().toUtf8().constData());
  fprintf(f,"\n");
  for(int i=0;i<SWITCHYARD_MAX_SLOTS;i++) {
    fprintf(f,"[Slot%u]\n",i+1);
    if(i<(int)srcSlots()) {
      fprintf(f,"SourceAddress=%s\n",
	      srcAddress(i).toString().toUtf8().constData());
      fprintf(f,"SourceName=%s\n",srcName(i).toUtf8().constData());
      fprintf(f,"SourceEnabled=%d\n",srcEnabled(i));
    }
    if(i<(int)dstSlots()) {
      fprintf(f,"DestinationAddress=%s\n",
	      dstAddress(i).toString().toUtf8().constData());
      fprintf(f,"DestinationName=%s\n",dstName(i).toUtf8().constData());
    }
    if(i<(int)gpos()) {
      fprintf(f,"GpoMode=%d\n",gpoMode(i));
      fprintf(f,"GpoAddress=%s\n",
	      gpoAddress(i).toString().toUtf8().constData());
      fprintf(f,"GpoSnakeSlot=%d\n",gpoSnakeSlot(i));
      fprintf(f,"GpoName=%s\n",gpoName(i).toUtf8().constData());
    }
    fprintf(f,"\n");
  }
  fclose(f);
  rename(tempfile.toUtf8(),SWITCHYARD_ROUTING_FILE);
#endif  // WIN32
}


void SyRouting::writeRtpData(unsigned src_slot,const char *data,int len) const
{
  struct sockaddr_in sa;

  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(SWITCHYARD_RTP_PORT);
  sa.sin_addr.s_addr=src_addr[src_slot];
  sendto(rtpSendSocket(),data,len,0,(struct sockaddr *)(&sa),sizeof(sa));
}


QHostAddress SyRouting::streamAddress(SyRouting::Realm realm,uint16_t lw_num)
{
  //
  // From page 113 of 'Intro to Livewire, v2.1.1'
  //
  QHostAddress ret;
  switch(realm) {
  case SyRouting::Stereo:    // 239.192.0.0/15
    ret.setAddress(4022337536u+lw_num);
    break;

  case SyRouting::Backfeed:  // 239.193.0.0/15
    ret.setAddress(4022403072u+lw_num);
    break;

  case SyRouting::Surround:  // 239.128.0.0/15
    ret.setAddress(4022599680u+lw_num);
    break;
  }

  return ret;
}


QString SyRouting::sourceString(const QHostAddress &s_addr,int s_slot)
{
  QString ret;

  if(s_addr.isNull()) {
    ret=QString::asprintf("%d",s_slot);
  }
  else {
    ret=s_addr.toString();
    if(s_slot>=0) {
      ret+=QString::asprintf("/%d",s_slot+1);
    }
  }

  return ret;
}


unsigned SyRouting::livewireNumber(const QHostAddress &addr)
{
  uint32_t srcmask=0xFFFF0000&addr.toIPv4Address();

  if((srcmask!=0xEFC00000)&&(srcmask!=0xEFC10000)&&(srcmask!=0xEFC40000)) {
    return 0;
  }
  return 0xFFFF&addr.toIPv4Address();
}


QString SyRouting::dumpAddress(uint32_t addr)
{
  QHostAddress a(ntohl(addr));
  return a.toString();
}


QString SyRouting::socketErrorString(const QString &msg)
{
  QString ret=msg;
#ifdef WIN32
  int err=WSAGetLastError();
  ret+=QString::asprintf(" [Winsock error: %d]",err);
#else
  ret+=QString(" [")+strerror(errno)+"]";
#endif  // WIN32

  return ret;
}


void SyRouting::LoadInterfaces()
{
  if((sy_subscription_socket=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    SySyslog(LOG_ERR,
      SyRouting::socketErrorString("unable to create RTP suscription socket"));
    exit(256);
  }
  if((sy_rtp_send_socket=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    SySyslog(LOG_ERR,
      SyRouting::socketErrorString("unable to create RTP send socket"));
    exit(256);
  } 

#ifdef OSX
  struct ifaddrs *ifap=NULL;
  struct ifaddrs *ifa=NULL;
  struct sockaddr_in *sa_in=NULL;

  if(getifaddrs(&ifap)<0) {
    SySyslog(LOG_ERR,QString().
	   sprintf("unable to get interface information [%s]",strerror(errno)));
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
#endif  // OSX
#ifdef LINUX
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
	  push_back(QString::asprintf("%s: %02X:%02X:%02X:%02X:%02X:%02X",
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
#endif  // LINUX
#ifdef WIN32
  IP_ADAPTER_INFO *info;
  ULONG buf_len;
  DWORD ret=0;

  info=(IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
  buf_len=sizeof(IP_ADAPTER_INFO);
  if(GetAdaptersInfo(info,&buf_len)!=ERROR_SUCCESS) {
    free(info);
    info=(IP_ADAPTER_INFO *)malloc(buf_len);
  }
  if(GetAdaptersInfo(info,&buf_len)!=ERROR_SUCCESS) {
    SySyslog(LOG_ERR,QString::asprintf("GetAdaptersInfo failed, error=%lu",ret));
    exit(256);
  }
  PIP_ADAPTER_INFO a=info;
  int count=0;
  while(a) {
    //printf("Name: %s\n",a->AdapterName);
    //printf("Desc: %s\n",a->Description);
    QString dev=QString::asprintf("%d: ",count+1);
    for(unsigned i=0;i<a->AddressLength;i++) {
      dev+=QString::asprintf("%02X:",(int)a->Address[i]);
    }
    dev=dev.left(dev.length()-1);
    sy_nic_devices.push_back(dev);
    sy_nic_addresses.push_back(QHostAddress(a->IpAddressList.IpAddress.String));
    sy_nic_netmasks.push_back(QHostAddress(a->IpAddressList.IpMask.String));
    count++;
    a=info->Next;
  }
  if(info!=NULL) {
    free(info);
  }
#endif  // WIN32
  /*
  for(unsigned i=0;i<sy_nic_addresses.size();i++) {
    printf("%s: %s  %s\n",(const char *)sy_nic_devices[i].toUtf8(),
	   (const char *)sy_nic_addresses[i].toString().toUtf8(),
	   (const char *)sy_nic_netmasks[i].toString().toUtf8());
  }
  */
}
