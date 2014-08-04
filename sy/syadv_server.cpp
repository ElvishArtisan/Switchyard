// syadv_server.cpp
//
// Livewire Advertising Protocol Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

#include <QtCore/QStringList>

#include "syadv_server.h"
#include "sylogger.h"

SyAdvServer::SyAdvServer(SyRouting *r,bool read_only,QObject *parent)
  : QObject(parent)
{
  adv_routing=r;
  ctrl_advert_type=SyAdvServer::Type0;

  //
  // Initialize Advertisment Socket
  //
  if(read_only) {
    ctrl_advert_socket=new SyMcastSocket(SyMcastSocket::ReadOnly,this);
    ctrl_advert_socket->bind(SWITCHYARD_ADVERTS_PORT);
  }
  else {
    ctrl_advert_socket=new SyMcastSocket(SyMcastSocket::ReadWrite,this);
    ctrl_advert_socket->bind(r->nicAddress(),SWITCHYARD_ADVERTS_PORT);
  }
  ctrl_advert_socket->subscribe(SWITCHYARD_ADVERTS_ADDRESS);
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


void SyAdvServer::readData()
{
  uint8_t data[1500];
  QHostAddress addr;
  uint16_t port;
  int n;
  SyAdvPacket p;
  SyAdvSource *src=NULL;
  int slot;
  SyAdvSource::HardwareType hwid=SyAdvSource::TypeUnknown;
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
	hwid=(SyAdvSource::HardwareType)p.tag(i)->tagValue().toUInt();
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


void SyAdvServer::expireData()
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


void SyAdvServer::sendData()
{
  int base=GetAdvertInterval();
  double stamp=GetTimestamp();

  SendSourceUpdate(ctrl_advert_type);
  ctrl_advert_type=(SyAdvServer::AdvertType)(ctrl_advert_type+1);
  if(ctrl_advert_type==SyAdvServer::TypeLast) {
    ctrl_advert_type=SyAdvServer::Type0;
  }
  if((ctrl_advert_timestamp-stamp)<16.0) {
    ctrl_advert_timestamp=stamp+150.0;
  }
  ctrl_advert_timer->start(base);
}


void SyAdvServer::saveSourcesData()
{
  QString tempfile=QString(SWITCHYARD_SOURCES_FILE)+"-temp";
  FILE *f=NULL;
  unsigned num=0;

  if((f=fopen(tempfile.toAscii(),"w"))==NULL) {
    SySyslog(LOG_WARNING,
	     QString().sprintf("unable to update sources database [%s]",
			       strerror(errno)));
    return;
  }

  for(unsigned i=0;i<ctrl_sources.size();i++) {
    SyAdvSource *src=ctrl_sources[i];
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
  SySyslog(LOG_DEBUG,
	   QString().sprintf("saved sources list to \"%s\"",
			     SWITCHYARD_SOURCES_FILE));
}


void SyAdvServer::SendSourceUpdate(AdvertType type)
{
  uint8_t data[1500];
  int n;

  SyAdvPacket *p=new SyAdvPacket();
  p->setSequenceNumber(ctrl_advert_seqno++);
  GenerateAdvertPacket(p,type);
  if((n=p->writePacket(data,1500))>0) {
    ctrl_advert_socket->writeDatagram((const char *)data,n,
				      QHostAddress(SWITCHYARD_ADVERTS_ADDRESS),
				      SWITCHYARD_ADVERTS_PORT);
  }
  else {
    SySyslog(LOG_WARNING,"invalid LWCP packet generated");
  }
  delete p;
}


void SyAdvServer::GenerateAdvertPacket(SyAdvPacket *p,AdvertType type) const
{
  //
  // FIXME: This breaks when used with more than eight active sources!
  //
  SyTag tag;
  char hostname[33];

  switch(type) {
  case SyAdvServer::Type0:
    tag.setTagName("NEST");
    tag.setTagValue(SyTag::TagType0,5+adv_routing->activeSources());
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(SyTag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(SyTag::TagType7,3);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(SyTag::TagType6,0x0D);  // number of bytes in following two tags
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(SyTag::TagType0,1);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(SyTag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    for(unsigned i=0;i<adv_routing->srcSlots();i++) {
      if((!adv_routing->srcAddress(i).isNull())&&
	 (adv_routing->srcAddress(i).toString()!="0.0.0.0")&&
	 adv_routing->srcEnabled(i)) {
	tag.setTagName(QString().sprintf("S%03u",i+1));
	tag.setTagValue(SyTag::TagType6,0x1C);    // number of bytes describing source
	p->addTag(tag);
	tag.setTagName("INDI");
	tag.setTagValue(SyTag::TagType0,2);
	p->addTag(tag);
	tag.setTagName("PSID");
	tag.setTagValue(SyTag::TagType1,adv_routing->srcAddress(i).toIPv4Address()&0xFFFF);
	p->addTag(tag);
	tag.setTagName("BUSY");
	tag.setTagValue(SyTag::TagType9,0);
	p->addTag(tag);
      }
    }
    break;

  case SyAdvServer::Type1:
    tag.setTagName("NEST");
    tag.setTagValue(SyTag::TagType0,3);
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(SyTag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(SyTag::TagType7,2);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(SyTag::TagType6,0x2D);  // Length!
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(SyTag::TagType0,5);
    p->addTag(tag);
    tag.setTagName("ADVV");
    tag.setTagValue(SyTag::TagType1,0x0A);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(SyTag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    tag.setTagName("INIP");
    tag.setTagValue(SyTag::TagType1,adv_routing->nicAddress());
    p->addTag(tag);
    tag.setTagName("UDPC");
    tag.setTagValue(SyTag::TagType8,4000);
    p->addTag(tag);
    tag.setTagName("NUMS");
    tag.setTagValue(SyTag::TagType8,1);
    p->addTag(tag);
    break;

  case SyAdvServer::Type2:
    tag.setTagName("NEST");
    tag.setTagValue(SyTag::TagType0,3+adv_routing->activeSources());
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(SyTag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(SyTag::TagType7,1);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(SyTag::TagType6,0x54);  // number of bytes to first source tag
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(SyTag::TagType0,6);
    p->addTag(tag);
    tag.setTagName("ADVV");  // ???
    tag.setTagValue(SyTag::TagType1,0x0A);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(SyTag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    tag.setTagName("INIP");
    tag.setTagValue(SyTag::TagType1,adv_routing->nicAddress());
    p->addTag(tag);
    tag.setTagName("UDPC");
    tag.setTagValue(SyTag::TagType8,4000);
    p->addTag(tag);
    tag.setTagName("NUMS");
    tag.setTagValue(SyTag::TagType8,adv_routing->activeSources());
    p->addTag(tag);
    gethostname(hostname,32);
    tag.setTagName("ATRN");
    tag.setTagValue(SyTag::TagString,QString(hostname).split(".")[0],32); // 32 characters, zero padded
    p->addTag(tag);

    //
    // One for each source
    //
    for(unsigned i=0;i<adv_routing->srcSlots();i++) {
      if((!adv_routing->srcAddress(i).isNull())&&
	 (adv_routing->srcAddress(i).toString()!="0.0.0.0")&&
	 adv_routing->srcEnabled(i)) {
	tag.setTagName(QString().sprintf("S%03u",i+1));
	tag.setTagValue(SyTag::TagType6,0x65);  // bytes in source record
	p->addTag(tag);
	tag.setTagName("INDI");
	tag.setTagValue(SyTag::TagType0,0x0B);
	p->addTag(tag);
	tag.setTagName("PSID");
	tag.setTagValue(SyTag::TagType1,adv_routing->srcNumber(i));
	p->addTag(tag);
	tag.setTagName("SHAB");
	tag.setTagValue(SyTag::TagType7,0);
	p->addTag(tag);
	tag.setTagName("FSID");
	tag.setTagValue(SyTag::TagType1,adv_routing->srcAddress(i));
	p->addTag(tag);
	tag.setTagName("FAST");
	tag.setTagValue(SyTag::TagType7,2);
	p->addTag(tag);
	tag.setTagName("FASM");
	tag.setTagValue(SyTag::TagType7,1);
	p->addTag(tag);
	tag.setTagName("BSID");
	tag.setTagValue(SyTag::TagType1,0);
	p->addTag(tag);
	tag.setTagName("BAST");
	tag.setTagValue(SyTag::TagType7,1);
	p->addTag(tag);
	tag.setTagName("BASM");
	tag.setTagValue(SyTag::TagType7,0);
	p->addTag(tag);
	tag.setTagName("LPID");
	tag.setTagValue(SyTag::TagType1,adv_routing->srcNumber(i));
	p->addTag(tag);
	tag.setTagName("STPL");
	tag.setTagValue(SyTag::TagType7,0);
	p->addTag(tag);
	tag.setTagName("PSNM");  // 16 characters, null padded
	tag.setTagValue(SyTag::TagString,adv_routing->srcName(i),16);
	p->addTag(tag);
      }
    }
    break;

  case SyAdvServer::TypeLast:
    break;
  }
}


int SyAdvServer::GetAdvertInterval() const
{
  long base=random()/(RAND_MAX/14)+2;
  return 1000*base+10*base;
}


SyAdvSource *SyAdvServer::GetSource(const QHostAddress &node_addr,
				     unsigned slot)
{
  SyAdvSource *src=NULL;

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
      ctrl_sources[i]=new SyAdvSource();
      src=ctrl_sources[i];
    }
  }
  if(src==NULL) {
    ctrl_sources.push_back(new SyAdvSource());
    src=ctrl_sources.back();
  }
  src->setNodeAddress(node_addr);
  src->setSlot(slot);
  return src;
}


int SyAdvServer::TagIsSource(const SyTag *tag) const
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


void SyAdvServer::ScheduleSourceSave()
{
  ctrl_savesources_timer->stop();
  ctrl_savesources_timer->start(SWITCHYARD_SAVESOURCES_INTERVAL);
}


double SyAdvServer::GetTimestamp() const
{
  struct timeval tv;
  memset(&tv,0,sizeof(tv));
  gettimeofday(&tv,NULL);
  return (double)tv.tv_sec+(double)tv.tv_usec/1000000.0;
}
