// adv_server.cpp
//
// Livewire Advertising Protocol Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include <QtCore/QStringList>

#include "adv_server.h"

AdvServer::AdvServer(Routing *r,bool read_only,QObject *parent)
  : QObject(parent)
{
  adv_routing=r;
  ctrl_advert_type=AdvServer::Type0;
  int sock;
  long sockopt;
  struct sockaddr_in sa;
  struct ip_mreqn mreq;
  QHostAddress addr;

  //
  // Initialize Advertisment Socket
  //
  if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    syslog(LOG_ERR,"unable to create advertisement socket [%s]",strerror(errno));
    exit(256);
  }
  sockopt=O_NONBLOCK;
  fcntl(sock,F_SETFL,sockopt);
  memset(&mreq,0,sizeof(mreq));
  addr.setAddress(SWITCHYARD_ADVERTS_ADDRESS);
  mreq.imr_multiaddr.s_addr=htonl(addr.toIPv4Address());
  mreq.imr_address.s_addr=htonl(adv_routing->nicAddress().toIPv4Address());
  mreq.imr_ifindex=0;
  if(setsockopt(sock,SOL_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
    syslog(LOG_ERR,"unable to subscribe to advert group [%s]",strerror(errno));
    exit(256);
  }
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(SWITCHYARD_ADVERTS_PORT);
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(sock,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind advertisment port [%s]",strerror(errno));
    exit(256);
  }
  ctrl_advert_socket=new QUdpSocket(this);
  ctrl_advert_socket->setSocketDescriptor(sock,QAbstractSocket::BoundState);
  connect(ctrl_advert_socket,SIGNAL(readyRead()),this,SLOT(readData()));

  //
  // Start Advertising
  //
  srandom(time(NULL));
  ctrl_advert_seqno=random();
  ctrl_advert_timestamp=GetTimestamp();
  ctrl_advert_timer=new QTimer(this);
  ctrl_advert_timer->setSingleShot(true);
  if(!read_only) {
    connect(ctrl_advert_timer,SIGNAL(timeout()),this,SLOT(sendData()));
    ctrl_advert_timer->start(GetAdvertInterval());
  }

  //
  // Start Timers
  //
  ctrl_expire_timer=new QTimer(this);
  connect(ctrl_expire_timer,SIGNAL(timeout()),this,SLOT(expireData()));
  ctrl_expire_timer->start(30000);

  ctrl_savesources_timer=new QTimer(this);
  ctrl_savesources_timer->setSingleShot(true);
  connect(ctrl_savesources_timer,SIGNAL(timeout()),
	  this,SLOT(saveSourcesData()));
}


void AdvServer::readData()
{
  uint8_t data[1500];
  QHostAddress addr;
  uint16_t port;
  int n;
  Packet p;
  Source *src=NULL;
  int slot;
  Source::HardwareType hwid=Source::TypeUnknown;
  QString nodename;

  while((n=ctrl_advert_socket->readDatagram((char *)data,1500,&addr,&port))>0) {
    p.readPacket(data,n);
    /*
    printf("************************************************************\n");
    printf("processing from %s: %s\n",
	   (const char *)addr.toString().toAscii(),
	   (const char *)p.dump().toAscii());
    */
    for(unsigned i=0;i<p.tags();i++) {
      if(p.tag(i)->tagName()=="HWID") {
	hwid=(Source::HardwareType)p.tag(i)->tagValue().toUInt();
      }
      if(p.tag(i)->tagName()=="ATRN") {  // Node Name
	nodename=p.tag(i)->tagValue().toString();
      }
      /*
      printf("  examining[%u]: %s\n",i,
	     (const char *)p.tag(i)->tagName().toAscii());
      */
      if((slot=TagIsSource(p.tag(i)))>=0) {
	src=GetSource(addr,slot);
	src->setHardwareType(hwid);
	while((++i<p.tags())&&(TagIsSource(p.tag(i))<0)) {
	  if(p.tag(i)->tagName()=="PSID") {  // Source Number
	    src->setSourceNumber(p.tag(i)->tagValue().toUInt());
	  }
	  if(p.tag(i)->tagName()=="FSID") {  // Stream Address
	    src->setStreamAddress(QHostAddress(p.tag(i)->tagValue().toUInt()));
	  }
	  if(p.tag(i)->tagName()=="PSNM") {  // Stream Name
	    src->setSourceName(p.tag(i)->tagValue().toString());
	  }
	  src->touch();
	}
	i--;
	if(src->isChanged()) {
	  ScheduleSourceSave();
	}
      }
    }
    if(!nodename.isEmpty()) {
      for(unsigned i=0;i<ctrl_sources.size();i++) {
	if((src=ctrl_sources[i])!=NULL) {
	  if(addr==src->nodeAddress()) {
	    src->setNodeName(nodename);
	    if(src->isChanged()) {
	      ScheduleSourceSave();
	    }
	  }
	}
      }
    }
  }
}


void AdvServer::expireData()
{
  QDateTime now=QDateTime(QDate::currentDate(),QTime::currentTime());
  for(unsigned i=0;i<ctrl_sources.size();i++) {
    if(ctrl_sources[i]!=NULL) {
      if(ctrl_sources[i]->lastTouched(now)>30) {
	delete ctrl_sources[i];
	ctrl_sources[i]=NULL;
	ScheduleSourceSave();
      }
    }
  }
}


void AdvServer::sendData()
{
  int base=GetAdvertInterval();
  double stamp=GetTimestamp();

  SendSourceUpdate(ctrl_advert_type);
  ctrl_advert_type=(AdvServer::AdvertType)(ctrl_advert_type+1);
  if(ctrl_advert_type==AdvServer::TypeLast) {
    ctrl_advert_type=AdvServer::Type0;
  }
  if((ctrl_advert_timestamp-stamp)<16.0) {
    ctrl_advert_timestamp=stamp+150.0;
  }
  ctrl_advert_timer->start(base);
}


void AdvServer::saveSourcesData()
{
  QString tempfile=QString(SWITCHYARD_SOURCES_FILE)+"-temp";
  FILE *f=NULL;
  unsigned num=0;

  if((f=fopen(tempfile.toAscii(),"w"))==NULL) {
    syslog(LOG_WARNING,"unable to update sources database [%s]",
	   strerror(errno));
    return;
  }

  for(unsigned i=0;i<ctrl_sources.size();i++) {
    Source *src=ctrl_sources[i];
    if(src!=NULL) {
      if(!src->streamAddress().isNull()) {
	fprintf(f,"[Source %u]\n",++num);
	fprintf(f,"Slot=%u\n",src->slot());
	fprintf(f,"NodeAddress=%s\n",
		(const char *)src->nodeAddress().toString().toAscii());
	fprintf(f,"NodeName=%s\n",(const char *)src->nodeName().toUtf8());
	fprintf(f,"StreamAddress=%s\n",
		(const char *)src->streamAddress().toString().toAscii());
	fprintf(f,"SourceName=%s\n",(const char *)src->sourceName().toUtf8());
	fprintf(f,"\n");
      }
    }
    src->setSaved();
  }

  fclose(f);
  rename(tempfile.toAscii(),SWITCHYARD_SOURCES_FILE);
  syslog(LOG_DEBUG,"saved sources list to \"%s\"",SWITCHYARD_SOURCES_FILE);
}


void AdvServer::SendSourceUpdate(AdvertType type)
{
  uint8_t data[1500];
  int n;

  Packet *p=new Packet();
  p->setSequenceNumber(ctrl_advert_seqno++);
  GenerateAdvertPacket(p,type);
  if((n=p->writePacket(data,1500))>0) {
    ctrl_advert_socket->writeDatagram((const char *)data,n,
				      QHostAddress(SWITCHYARD_ADVERTS_ADDRESS),
				      SWITCHYARD_ADVERTS_PORT);
  }
  else {
    syslog(LOG_WARNING,"invalid LWCP packet generated");
  }
  delete p;
}


void AdvServer::GenerateAdvertPacket(Packet *p,AdvertType type) const
{
  //
  // FIXME: This breaks when used with more than eight active sources!
  //
  Tag tag;
  char hostname[33];

  switch(type) {
  case AdvServer::Type0:
    tag.setTagName("NEST");
    tag.setTagValue(Tag::TagType0,5+adv_routing->activeSources());
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(Tag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(Tag::TagType7,3);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(Tag::TagType6,0x0D);  // number of bytes in following two tags
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(Tag::TagType0,1);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(Tag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    for(unsigned i=0;i<SWITCHYARD_SLOTS;i++) {
      if((!adv_routing->srcAddress(i).isNull())&&
	 (adv_routing->srcAddress(i).toString()!="0.0.0.0")&&
	 adv_routing->srcEnabled(i)) {
	tag.setTagName(QString().sprintf("S%03u",i+1));
	tag.setTagValue(Tag::TagType6,0x1C);    // number of bytes describing source
	p->addTag(tag);
	tag.setTagName("INDI");
	tag.setTagValue(Tag::TagType0,2);
	p->addTag(tag);
	tag.setTagName("PSID");
	tag.setTagValue(Tag::TagType1,adv_routing->srcAddress(i).toIPv4Address()&0xFFFF);
	p->addTag(tag);
	tag.setTagName("BUSY");
	tag.setTagValue(Tag::TagType9,0);
	p->addTag(tag);
      }
    }
    break;

  case AdvServer::Type1:
    tag.setTagName("NEST");
    tag.setTagValue(Tag::TagType0,3);
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(Tag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(Tag::TagType7,2);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(Tag::TagType6,0x2D);  // Length!
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(Tag::TagType0,5);
    p->addTag(tag);
    tag.setTagName("ADVV");
    tag.setTagValue(Tag::TagType1,0x0A);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(Tag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    tag.setTagName("INIP");
    tag.setTagValue(Tag::TagType1,adv_routing->nicAddress());
    p->addTag(tag);
    tag.setTagName("UDPC");
    tag.setTagValue(Tag::TagType8,4000);
    p->addTag(tag);
    tag.setTagName("NUMS");
    tag.setTagValue(Tag::TagType8,1);
    p->addTag(tag);
    break;

  case AdvServer::Type2:
    tag.setTagName("NEST");
    tag.setTagValue(Tag::TagType0,3+adv_routing->activeSources());
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(Tag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(Tag::TagType7,1);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(Tag::TagType6,0x54);  // number of bytes to first source tag
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(Tag::TagType0,6);
    p->addTag(tag);
    tag.setTagName("ADVV");  // ???
    tag.setTagValue(Tag::TagType1,0x0A);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(Tag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    tag.setTagName("INIP");
    tag.setTagValue(Tag::TagType1,adv_routing->nicAddress());
    p->addTag(tag);
    tag.setTagName("UDPC");
    tag.setTagValue(Tag::TagType8,4000);
    p->addTag(tag);
    tag.setTagName("NUMS");
    tag.setTagValue(Tag::TagType8,adv_routing->activeSources());
    p->addTag(tag);
    gethostname(hostname,32);
    tag.setTagName("ATRN");
    tag.setTagValue(Tag::TagString,QString(hostname).split(".")[0],32); // 32 characters, zero padded
    p->addTag(tag);

    //
    // One for each source
    //
    for(unsigned i=0;i<SWITCHYARD_SLOTS;i++) {
      if((!adv_routing->srcAddress(i).isNull())&&
	 (adv_routing->srcAddress(i).toString()!="0.0.0.0")&&
	 adv_routing->srcEnabled(i)) {
	tag.setTagName(QString().sprintf("S%03u",i+1));
	tag.setTagValue(Tag::TagType6,0x65);  // bytes in source record
	p->addTag(tag);
	tag.setTagName("INDI");
	tag.setTagValue(Tag::TagType0,0x0B);
	p->addTag(tag);
	tag.setTagName("PSID");
	tag.setTagValue(Tag::TagType1,adv_routing->srcNumber(i));
	p->addTag(tag);
	tag.setTagName("SHAB");
	tag.setTagValue(Tag::TagType7,0);
	p->addTag(tag);
	tag.setTagName("FSID");
	tag.setTagValue(Tag::TagType1,adv_routing->srcAddress(i));
	p->addTag(tag);
	tag.setTagName("FAST");
	tag.setTagValue(Tag::TagType7,2);
	p->addTag(tag);
	tag.setTagName("FASM");
	tag.setTagValue(Tag::TagType7,1);
	p->addTag(tag);
	tag.setTagName("BSID");
	tag.setTagValue(Tag::TagType1,0);
	p->addTag(tag);
	tag.setTagName("BAST");
	tag.setTagValue(Tag::TagType7,1);
	p->addTag(tag);
	tag.setTagName("BASM");
	tag.setTagValue(Tag::TagType7,0);
	p->addTag(tag);
	tag.setTagName("LPID");
	tag.setTagValue(Tag::TagType1,adv_routing->srcNumber(i));
	p->addTag(tag);
	tag.setTagName("STPL");
	tag.setTagValue(Tag::TagType7,0);
	p->addTag(tag);
	tag.setTagName("PSNM");  // 16 characters, null padded
	tag.setTagValue(Tag::TagString,adv_routing->srcName(i),16);
	p->addTag(tag);
      }
    }
    break;

  case AdvServer::TypeLast:
    break;
  }
}


int AdvServer::GetAdvertInterval() const
{
  long base=random()/(RAND_MAX/14)+2;
  return 1000*base+10*base;
}


Source *AdvServer::GetSource(const QHostAddress &node_addr,
				     unsigned slot)
{
  Source *src=NULL;

  for(unsigned i=0;i<ctrl_sources.size();i++) {
    if(ctrl_sources[i]!=NULL) {
      if((ctrl_sources[i]->nodeAddress()==node_addr)&&
	 (ctrl_sources[i]->slot()==slot)) {
	return ctrl_sources[i];
      }
    }
  }
  for(unsigned i=0;i<ctrl_sources.size();i++) {
    if(ctrl_sources[i]==NULL) {
      ctrl_sources[i]=new Source();
      src=ctrl_sources[i];
    }
  }
  if(src==NULL) {
    ctrl_sources.push_back(new Source());
    src=ctrl_sources.back();
  }
  src->setNodeAddress(node_addr);
  src->setSlot(slot);
  return src;
}


int AdvServer::TagIsSource(const Tag *tag) const
{
  int slot;
  bool ok;

  if(tag->tagName().left(1)=="S") {
    slot=tag->tagName().right(3).toUInt(&ok);
    if(ok) {
      return slot;
    }
  }
  return -1;
}


void AdvServer::ScheduleSourceSave()
{
  ctrl_savesources_timer->stop();
  ctrl_savesources_timer->start(SWITCHYARD_SAVESOURCES_INTERVAL);
}


double AdvServer::GetTimestamp() const
{
  struct timeval tv;
  memset(&tv,0,sizeof(tv));
  gettimeofday(&tv,NULL);
  return (double)tv.tv_sec+(double)tv.tv_usec/1000000.0;
}
