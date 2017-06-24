// syadv_server.cpp
//
// Livewire Advertising Protocol Server
//
// (C) Copyright 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef WIN32
#include <Winsock2.h>
#endif  // WIN32

#include <QSettings>
#include <QStringList>

#include "syadv_server.h"
#include "sysyslog.h"

//
// LiveWire advertising uses three different types of packets:
//
// Type 0 - Sent every ten seconds
//
// Type 1 - Sent at random intervals, between 6 and 20 seconds
//
// Type 2 - Sent every 150 seconds, as well as immediately at service startup.
//

SyAdvServer::SyAdvServer(SyRouting *r,bool read_only,QObject *parent)
  : QObject(parent)
{
  adv_routing=r;
  adv_eth_monitor=NULL;

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

  Initialize(read_only);
}


SyAdvServer::SyAdvServer(SyRouting *r,SyEthMonitor *ethmon,bool read_only,
			 QObject *parent)
  : QObject(parent)
{
  adv_routing=r;
  adv_eth_monitor=ethmon;
  connect(adv_eth_monitor,SIGNAL(startedRunning()),
	  this,SLOT(interfaceStartedData()));
  connect(adv_eth_monitor,SIGNAL(stoppedRunning()),
	  this,SLOT(interfaceStopeedData()));

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
  if(adv_eth_monitor->isRunning()) {
    ctrl_advert_socket->subscribe(SWITCHYARD_ADVERTS_ADDRESS);
  }
  connect(ctrl_advert_socket,SIGNAL(readyRead()),this,SLOT(readData()));

  Initialize(read_only);
}


SyAdvServer::~SyAdvServer()
{
  //
  // Delete Source Table
  //
#ifdef WIN32
  QSettings *s= new QSettings(QSettings::SystemScope,
			      SWITCHYARD_SETTINGS_ORGANIZATION,
			      SWITCHYARD_SETTINGS_APPLICATION);

  QStringList keys=s->allKeys();
  for(int i=0;i<keys.size();i++) {
    QStringList f0=keys[i].split(" ");
    if((f0[0]=="Source")&&(f0.size()==2)) {
      s->remove(keys[i]);
    }
  }
  delete s;
#else
  unlink(SWITCHYARD_SOURCES_FILE);
#endif  // WIN32
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


void SyAdvServer::sendAdvert0Data()
{
  SendSourceUpdate(SyAdvServer::Type0);
}


void SyAdvServer::sendAdvert1Data()
{
  SendSourceUpdate(SyAdvServer::Type1);
  ctrl_advert_timer1->start(GetAdvertInterval());
}


void SyAdvServer::sendAdvert2Data()
{
  SendSourceUpdate(SyAdvServer::Type2);
}


void SyAdvServer::saveSourcesData()
{
#ifdef WIN32
  unsigned num=0;
  QSettings *s= new QSettings(QSettings::SystemScope,
			      SWITCHYARD_SETTINGS_ORGANIZATION,
			      SWITCHYARD_SETTINGS_APPLICATION);

  //
  // Write current sources
  //
  for(unsigned i=0;i<ctrl_sources.size();i++) {
    SyAdvSource *src=ctrl_sources[i];
    if(src!=NULL) {
      QString key=QString().sprintf("Source %u",++num);
      s->setValue(key+"/Slot",src->slot());
      s->setValue(key+"/NodeAddress",src->nodeAddress().toString());
      s->setValue(key+"/NodeName",src->nodeName());
      s->setValue(key+"/StreamAddress",src->streamAddress().toString());
      s->setValue(key+"/SourceName",src->sourceName());
      src->setSaved();
    }  
  }

  //
  // Purge stale sources
  //
  QStringList keys=s->allKeys();
  for(int i=0;i<keys.size();i++) {
    QStringList f0=keys[i].split(" ");
    if((f0[0]=="Source")&&(f0.size()==2)) {
      if(f0[1].toUInt()>=num) {
	s->remove(keys[i]);
      }
    }
  }

  delete s;
#else
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
      src->setSaved();
    }
  }

  fclose(f);
  rename(tempfile.toAscii(),SWITCHYARD_SOURCES_FILE);
  SySyslog(LOG_DEBUG,
	   QString().sprintf("saved sources list to \"%s\"",
			     SWITCHYARD_SOURCES_FILE));
#endif  // WIN32
}


void SyAdvServer::interfaceStartedData()
{
  ctrl_advert_socket->subscribe(SWITCHYARD_ADVERTS_ADDRESS);
}


void SyAdvServer::interfaceStoppedData()
{
  ctrl_advert_socket->unsubscribe(SWITCHYARD_ADVERTS_ADDRESS);
}


void SyAdvServer::SendSourceUpdate(AdvertType type)
{
  uint8_t data[1500];
  int n;

  SyAdvPacket *p=new SyAdvPacket();
  p->setSequenceNumber(ctrl_advert_seqno++);
  switch(type) {
  case SyAdvServer::Type0:
    GenerateAdvertPacket0(p);
    if((n=p->writePacket(data,1500))>0) {
      ctrl_advert_socket->writeDatagram((const char *)data,n,
					QHostAddress(SWITCHYARD_ADVERTS_ADDRESS),
					SWITCHYARD_ADVERTS_PORT);
    }
    else {
      SySyslog(LOG_WARNING,"invalid LWCP packet generated");
    }
    break;

  case SyAdvServer::Type1:
    GenerateAdvertPacket1(p);
    if((n=p->writePacket(data,1500))>0) {
      ctrl_advert_socket->writeDatagram((const char *)data,n,
					QHostAddress(SWITCHYARD_ADVERTS_ADDRESS),
					SWITCHYARD_ADVERTS_PORT);
    }
    else {
      SySyslog(LOG_WARNING,"invalid LWCP packet generated");
    }
    break;

  case SyAdvServer::Type2:
    for(unsigned i=0;i<adv_routing->srcSlots();i+=8) {
      if(GenerateAdvertPacket2(p,i)) {
	if((n=p->writePacket(data,1500))>0) {
	  ctrl_advert_socket->writeDatagram((const char *)data,n,
					    QHostAddress(SWITCHYARD_ADVERTS_ADDRESS),
					    SWITCHYARD_ADVERTS_PORT);
	}
	else {
	  SySyslog(LOG_WARNING,"invalid LWCP packet generated");
	}
      }
      delete p;
      p=new SyAdvPacket();
      p->setSequenceNumber(ctrl_advert_seqno++);
    }
    break;

  case SyAdvServer::TypeLast:
    break;
  }
  delete p;
}


void SyAdvServer::GenerateAdvertPacket0(SyAdvPacket *p) const
{
  //
  // Length is 16 + 39 + 35*num_of_srcs (41 srcs max)
  //
  SyTag tag;

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
}


void SyAdvServer::GenerateAdvertPacket1(SyAdvPacket *p) const
{
  //
  // Length is 87
  //
  SyTag tag;

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
}


bool SyAdvServer::GenerateAdvertPacket2(SyAdvPacket *p,unsigned base_slot) const
{
  SyTag tag;
  char hostname[33];
  unsigned count=0;

  //
  // Get number of active sources
  //
  for(unsigned i=base_slot;i<(base_slot+8);i++) {
    if((i<adv_routing->srcSlots())&&
       (!adv_routing->srcAddress(i).isNull())&&
       (adv_routing->srcAddress(i).toString()!="0.0.0.0")&&
       adv_routing->srcEnabled(i)) {
      count++;
    }
  }

  //
  // Generate Packet
  //
  tag.setTagName("NEST");
  tag.setTagValue(SyTag::TagType0,3+count);
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
  tag.setTagName("ADVV");  // Version
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
  for(unsigned i=base_slot;i<(base_slot+8);i++) {
    if((i<adv_routing->srcSlots())&&
       (!adv_routing->srcAddress(i).isNull())&&
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
  return count>0;
}


int SyAdvServer::GetAdvertInterval() const
{
#ifdef WIN32
  long base=rand()/(RAND_MAX/14)+2;
#else
  long base=random()/(RAND_MAX/14)+2;
#endif  // WIN32

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


void SyAdvServer::Initialize(bool read_only)
{
  //
  // Start Advertising
  //
#ifdef WIN32
  srand(static_cast<unsigned int> (time(NULL)));
  ctrl_advert_seqno=rand();
#else
  srandom(time(NULL));
  ctrl_advert_seqno=random();
#endif  // WIN32
  ctrl_advert_timer0=new QTimer(this);
  ctrl_advert_timer2=new QTimer(this);
  ctrl_advert_timer1=new QTimer(this);
  ctrl_advert_timer1->setSingleShot(true);
  if(!read_only) {
    connect(ctrl_advert_timer0,SIGNAL(timeout()),this,SLOT(sendAdvert0Data()));
    ctrl_advert_timer0->start(10000);
    connect(ctrl_advert_timer1,SIGNAL(timeout()),this,SLOT(sendAdvert1Data()));
    ctrl_advert_timer1->start(GetAdvertInterval());
    connect(ctrl_advert_timer2,SIGNAL(timeout()),this,SLOT(sendAdvert2Data()));
    ctrl_advert_timer2->start(150000);
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

  SendSourceUpdate(SyAdvServer::Type2);
}
