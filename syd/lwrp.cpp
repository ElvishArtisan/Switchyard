// lwrp.cpp
//
// Livewire Control Protocol
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <syslog.h>
#include <unistd.h>

#include "lwrp.h"

LWRPServer::LWRPServer(Routing *routing)
  : QObject()
{
  struct ip_mreqn mreq;
  struct sockaddr_in sa;
  long sockopt;
  int sock;
  QHostAddress addr;

  ctrl_advert_type=LWRPServer::Type0;
  ctrl_routing=routing;

  //
  // Initialize LWRP Server
  //
  ctrl_read_mapper=new QSignalMapper(this);
  connect(ctrl_read_mapper,SIGNAL(mapped(int)),this,SLOT(readData(int)));
  ctrl_closed_mapper=new QSignalMapper(this);
  connect(ctrl_closed_mapper,SIGNAL(mapped(int)),this,SLOT(closedData(int)));
  ctrl_server=new QTcpServer(this);
  connect(ctrl_server,SIGNAL(newConnection()),this,SLOT(newConnectionData()));
  if (!ctrl_server->listen(QHostAddress::Any,SWITCHYARD_LWRP_PORT)){
    syslog(LOG_ERR,"unable to bind port %d",SWITCHYARD_LWRP_PORT);
    exit(256);
  }

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
  mreq.imr_address.s_addr=htonl(ctrl_routing->nicAddress().toIPv4Address());
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
  connect(ctrl_advert_socket,SIGNAL(readyRead()),this,SLOT(advertReadData()));

  //
  // Initialize Clock Socket
  //
  if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    syslog(LOG_ERR,"unable to create clock socket [%s]",strerror(errno));
    exit(256);
  }
  sockopt=O_NONBLOCK;
  fcntl(sock,F_SETFL,sockopt);
  memset(&mreq,0,sizeof(mreq));
  mreq.imr_multiaddr.s_addr=htonl(QHostAddress(SWITCHYARD_CLOCK_ADDRESS).toIPv4Address());
  mreq.imr_address.s_addr=htonl(ctrl_routing->nicAddress().toIPv4Address());
  mreq.imr_ifindex=0;
  if(setsockopt(sock,SOL_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
    syslog(LOG_ERR,"unable to subscribe to clock stream [%s]",strerror(errno));
    exit(256);
  }
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(SWITCHYARD_CLOCK_PORT);
  sa.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(sock,(struct sockaddr *)&sa,sizeof(sa))<0) {
    syslog(LOG_ERR,"unable to bind clock port [%s]",strerror(errno));
    exit(256);
  }
  ctrl_clock_socket=new QUdpSocket(this);
  ctrl_clock_socket->setSocketDescriptor(sock,QAbstractSocket::BoundState);
  connect(ctrl_clock_socket,SIGNAL(readyRead()),this,SLOT(clockReadData()));

  //
  // Initialize slots
  //
  for(unsigned i=0;i<SWITCHYARD_SLOTS;i++) {
    ctrl_routing->subscribe(ctrl_routing->srcAddress(i));
  }

  //
  // Start Timers
  //
  ctrl_expire_timer=new QTimer(this);
  connect(ctrl_expire_timer,SIGNAL(timeout()),this,SLOT(advertExpireData()));
  ctrl_expire_timer->start(30000);

  //
  // Start Advertising
  //
  srandom(time(NULL));
  ctrl_advert_seqno=random();
  ctrl_advert_timestamp=GetTimestamp();
  //lw_name_next32=false;
  ctrl_advert_timer=new QTimer(this);
  ctrl_advert_timer->setSingleShot(true);
  connect(ctrl_advert_timer,SIGNAL(timeout()),this,SLOT(advertSendData()));
  ctrl_advert_timer->start(GetAdvertInterval());
}


void LWRPServer::newConnectionData()
{
  unsigned id=0;

  //
  // Get a slot
  //
  while((id<ctrl_client_connections.size())&&
	(ctrl_client_connections[id]!=NULL)) {
    id++;
  }
  if(id==ctrl_client_connections.size()) {  // Slots full, allocate a new one
    ctrl_client_connections.push_back(NULL);
  }

  //
  // Process the connection
  //
  ctrl_client_connections[id]=new ClientConnection();
  ctrl_client_connections[id]->setSocket(ctrl_server->nextPendingConnection());
  ctrl_read_mapper->setMapping (ctrl_client_connections[id]->socket(),id);
  connect(ctrl_client_connections[id]->socket(),SIGNAL(readyRead()),
	  ctrl_read_mapper,SLOT(map()));
  ctrl_closed_mapper->setMapping (ctrl_client_connections[id]->socket(),id);
  connect(ctrl_client_connections[id]->socket(),SIGNAL(connectionClosed()),
	  ctrl_closed_mapper,SLOT(map()));
}


void LWRPServer::readData(int id)
{
  char data[1500];
  int n;

  while((n=ctrl_client_connections[id]->socket()->read(data,1500))>0) {
    data[n]=0;
    for(int i=0;i<n;i++) {
      if(data[i]==10) { // End of line
	ParseCommand(id);
	ctrl_client_connections[id]->clearBuffer();
      }
      else {
	if(isprint(data[i])!=0) {
	  ctrl_client_connections[id]->appendCommandBuffer(data[i]);
	}
      }
    }
  }
}


void LWRPServer::advertReadData()
{
  uint8_t data[1500];
  QHostAddress addr;
  uint16_t port;
  int n;
  LwPacket p;
  LwSource *src=NULL;
  int slot;
  LwSource::HardwareType hwid=LwSource::TypeUnknown;

  while((n=ctrl_advert_socket->readDatagram((char *)data,1500,&addr,&port))>0) {
    p.readPacket(data,n);
    for(unsigned i=0;i<p.tags();i++) {
      if(p.tag(i)->tagName()=="HWID") {
	hwid=(LwSource::HardwareType)p.tag(i)->tagValue().toUInt();
      }
      if((slot=TagIsSource(p.tag(i)))>=0) {
	src=GetSource(addr,slot);
	src->setHardwareType(hwid);
	while((++i<p.tags())&&(TagIsSource(p.tag(i))<0)) {
	  if(p.tag(i)->tagName()=="PSID") {  // Source Number
	    src->setSourceNumber(p.tag(i)->tagValue().toUInt());
	    if(src->streamAddress().isNull()) {
	      src->setStreamAddress(QHostAddress(4022337536|
						 src->sourceNumber()));
	    }
	  }
	  if(p.tag(i)->tagName()=="FSID") {  // Stream Address
	    src->setStreamAddress(QHostAddress(p.tag(i)->tagValue().toUInt()));
	  }
	  src->touch();
	}
      }
    }
  }
}


void LWRPServer::advertExpireData()
{
  QDateTime now=QDateTime(QDate::currentDate(),QTime::currentTime());
  for(unsigned i=0;i<ctrl_sources.size();i++) {
    if(ctrl_sources[i]!=NULL) {
      if(ctrl_sources[i]->lastTouched(now)>30) {
	delete ctrl_sources[i];
	ctrl_sources[i]=NULL;
      }
    }
  }
}


void LWRPServer::advertSendData()
{
  int base=GetAdvertInterval();
  double stamp=GetTimestamp();
  uint8_t data[1500];
  int n;

  LwPacket *p=new LwPacket();
  p->setSequenceNumber(ctrl_advert_seqno++);
  GenerateAdvertPacket(p,ctrl_advert_type);
  if((n=p->writePacket(data,1500))>0) {
  ctrl_advert_socket->
    writeDatagram((const char *)data,n,QHostAddress(SWITCHYARD_ADVERTS_ADDRESS),
		  SWITCHYARD_ADVERTS_PORT);
  }
  else {
    fprintf(stderr,"lwcd: invalid LWCP packet generated\n");
  }
  delete p;
  ctrl_advert_type=(LWRPServer::AdvertType)(ctrl_advert_type+1);
  if(ctrl_advert_type==LWRPServer::TypeLast) {
    ctrl_advert_type=LWRPServer::Type0;
  }
  if((ctrl_advert_timestamp-stamp)<16.0) {
    ctrl_advert_timestamp=stamp+150.0;
  }
  ctrl_advert_timer->start(base);
}


void LWRPServer::clockReadData()
{
  uint8_t data[1500];
  QHostAddress addr;
  uint16_t port;

  while(ctrl_clock_socket->readDatagram((char *)data,1500,&addr,&port)>0) {
    ctrl_routing->setClkAddress(addr);
  }
}


void LWRPServer::clockStartData()
{
  /*
  LwSource *src=GetClockSource();
  if(src==NULL) {
    fprintf(stderr,"no suitable clock stream found\n");
    exit(256);
  }
  lw_shm->shm_clock_addr=htonl(src->streamAddress().toIPv4Address());
  LwSubscribe(ctrl_advert_socket->socketDescriptor(),lw_shm->shm_clock_addr,
	      htonl(ctrl_routing->nicAddress().toIPv4Address()));
  fprintf(stderr,"using %s from %s node at %s for clock\n",
	  (const char *)src->streamAddress().toString().toAscii(),
	  (const char *)LwSource::hardwareString(src->hardwareType()).toAscii(),
	  (const char *)src->nodeAddress().toString().toAscii());
  */
}


void LWRPServer::closedData(int id)
{
  delete ctrl_client_connections[id];
  ctrl_client_connections[id]=NULL;
}


bool LWRPServer::ExecuteLogin(int id,QStringList &args)
{
  return true;
}


bool LWRPServer::ExecuteVer(int id,QStringList &args)
{
  SendCommand(id,QString().
     sprintf("VER LWRP:%s DEVN:\"%s\" SYSV:%s NSRC:%u/2 NDST:%u NGPI:0 NGPO:0",
	     SWITCHYARD_PROTOCOL_VERSION,SWITCHYARD_DEVICE_NAME,VERSION,
	     SWITCHYARD_SLOTS,SWITCHYARD_SLOTS));
  return true;
}


bool LWRPServer::ExecuteIp(int ch,QStringList &args)
{
  /*
  SendCommand93(ch,QString().
		sprintf("IP address %s netmask %s gateway %s hostname %s",
			(const char *)ctrl_routing->nicAddress().toString().
			toAscii(),
			(const char *)lw_config->ipNetmask().toString().
			toAscii(),
			(const char *)lw_config->ipGateway().toString().
			toAscii(),
			(const char *)lw_config->hostName().toAscii()));
  */
  return true;
}


bool LWRPServer::ExecuteSrc(int id,QStringList &args)
{
  bool found=true;
  QStringList fields;
  QString rtpa;
  int rtpe=-1;
  QString psnm;
  int slot=0;
  bool ok;
  bool save=false;

  if(args.size()==1) {
    SendCommand(id,"BEGIN");
    for(unsigned i=0;i<SWITCHYARD_SLOTS;i++) {
      SendCommand(id,SrcLine(i));
    }
    SendCommand(id,"END");
  }
  else {
    slot=args[1].toInt(&ok)-1;
    if((!ok)|(slot<0)||(slot>=SWITCHYARD_SLOTS)) {
      return false;
    }
    for(int i=2;i<args.size();i++) {
      found=false;
      fields=ParseField(AString(args[i]));
      if(fields[0]=="RTPE") {
	rtpe=fields[1].toUInt();
	found=true;
      }
      if(fields[0]=="PSNM") {
	psnm=fields[1];
	found=true;
      }
      if(fields[0]=="RTPA") {
	rtpa=fields[1];
	found=true;
      }
      if(fields[0]=="INGN") {
	found=true;
      }
      if(fields[0]=="SHAB") {
	found=true;
      }
      if(fields[0]=="NCHN") {
	found=true;
      }
      if(fields[0]=="RTPP") {
	found=true;
      }
      if(!found) {
	return false;
      }
    }
    if(!rtpa.isNull()) {
      ctrl_routing->setSrcAddress(slot,rtpa);
      save=true;
    }
    if(!psnm.isNull()) {
      ctrl_routing->setSrcName(slot,psnm);
      save=true;
    }
    if(rtpe>=0) {
      ctrl_routing->setSrcEnabled(slot,rtpe);
      save=true;
    }
    BroadcastCommand(SrcLine(slot));
    if(save) {
      ctrl_routing->save();
    }
  }

  return true;
}


bool LWRPServer::ExecuteDst(int id,QStringList &args)
{
  bool found=true;
  QStringList fields;
  QStringList addrs;
  QString addr;
  QString name;
  int slot=0;
  bool ok;
  int streamno;
  bool save=false;

  if(args.size()==1) {
    SendCommand(id,"BEGIN");
    for(unsigned i=0;i<SWITCHYARD_SLOTS;i++) {
      SendCommand(id,DstLine(i));
    }
    SendCommand(id,"END");
  }
  else {
    slot=args[1].toInt(&ok)-1;
    if((!ok)|(slot<0)||(slot>=SWITCHYARD_SLOTS)) {
      return false;
    }
    for(int i=2;i<args.size();i++) {
      found=false;
      fields=ParseField(AString(args[i]));
      if(fields[0]=="ADDR") {
	addrs=fields[1].split("<");
	if(addrs[0].split(".").size()==4) {
	  addr=addrs[0].simplified();
	  found=true;
	}
	else {
	  streamno=addrs[0].toInt(&ok);
	  if(ok) {
	    if((streamno>0)&&(streamno<=0xFFFF)) {
	      addr=QString().sprintf("239.192.%d.%d",streamno/256,streamno%256);
	      found=true;
	    }
	    if(streamno==0) {
	      addr="0.0.0.0";
	      found=true;
	    }
	  }
	}
      }
      if(fields[0]=="NAME") {
	name=fields[1];
	found=true;
      }
      if(fields[0]=="NCHN") {
	found=true;
      }
      if(fields[0]=="LOAD") {
	found=true;
      }
      if(fields[0]=="OUGN") {
	found=true;
      }
      if(!found) {
	return false;
      }
    }
    if(!addr.isNull()) {
      if(addr!=ctrl_routing->dstAddress(slot).toString()) {
	UnsubscribeStream(slot);
	ctrl_routing->setDstAddress(slot,QHostAddress(addr));
	ctrl_routing->subscribe(ctrl_routing->dstAddress(slot));
	save=true;
      }
    }
    if(!name.isNull()) {
      ctrl_routing->setDstName(slot,name);
      save=true;
    }
    BroadcastCommand(DstLine(slot));
    if(save) {
      ctrl_routing->save();
    }
  }

  return true;
}


QString LWRPServer::SrcLine(int slot)
{
  return QString().
    sprintf("SRC %u PSNM:\"%s\" FASM:1 RTPE:%d RTPA:\"%s\" INGN:0 SHAB:0 NCHN:2 RTPP:240",
	    slot+1,
	    (const char *)ctrl_routing->srcName(slot).toAscii(),
	    ctrl_routing->srcEnabled(slot),
	    (const char *)ctrl_routing->srcAddress(slot).toString().toAscii());
}


QString LWRPServer::DstLine(int slot)
{
  return QString().
    sprintf("DST %u NAME:\"%s\" ADDR:\"%s\" NCHN:2 LOAD:0 OUGN:0",
	    slot+1,
	    (const char *)ctrl_routing->dstName(slot).toAscii(),
	    (const char *)ctrl_routing->dstAddress(slot).toString().toAscii());
}


QStringList LWRPServer::ParseField(const AString &field)
{
  QStringList ret=field.split(":");
  if(ret.size()>2) {
    for(int i=2;i<ret.size();i++) {
      ret[1]+=":"+ret[i];
    }
  }
  ret[0]=ret[0].toUpper();
  if(ret.size()<2) {
    ret.push_back("");
  }
  if(ret[1].left(1)=="\"") {
    ret[1]=ret[1].right(ret[1].length()-1);
  }
  if(ret[1].right(1)=="\"") {
    ret[1]=ret[1].left(ret[1].length()-1);
  }

  return ret;
}


void LWRPServer::ParseCommand(int id)
{
  bool ok=false;
  QStringList args=
    AString(ctrl_client_connections[id]->commandBuffer()).split(" ","\"");

  args[0]=args[0].toLower();
  if(args[0]=="login") {
    ok=ExecuteLogin(id,args);
  }
  if(args[0]=="ver") {
    ok=ExecuteVer(id,args);
  }
  if(args[0]=="ip") {
    ok=ExecuteIp(id,args);
  }
  if(args[0]=="src") {
    ok=ExecuteSrc(id,args);
  }
  if(args[0]=="dst") {
    ok=ExecuteDst(id,args);
  }
  if(!ok) {
    SendCommand(id,"ERROR 1000 bad command");
  }
}


void LWRPServer::BroadcastCommand(const QString &cmd)
{
  for(unsigned i=0;i<ctrl_client_connections.size();i++) {
    if(ctrl_client_connections[i]!=NULL) {
      SendCommand(i,cmd);
    }
  }
}


void LWRPServer::SendCommand(int ch,const QString &cmd)
{
  ctrl_client_connections[ch]->
    socket()->write((cmd+"\r\n").toAscii(),cmd.length()+2);
}


void LWRPServer::UnsubscribeStream(int slot)
{
  if(ctrl_routing->clkAddress()==ctrl_routing->dstAddress(slot)) {
    return;
  }
  for(int i=0;i<SWITCHYARD_SLOTS;i++) {
    if((ctrl_routing->dstAddress(i)==ctrl_routing->dstAddress(slot))&&
       (i!=slot)) {
      return;
    }
  }
  ctrl_routing->unsubscribe(ctrl_routing->dstAddress(slot));
}


LwSource *LWRPServer::GetSource(const QHostAddress &node_addr,
				     unsigned slot)
{
  LwSource *src=NULL;

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
      ctrl_sources[i]=new LwSource();
      src=ctrl_sources[i];
    }
  }
  if(src==NULL) {
    ctrl_sources.push_back(new LwSource());
    src=ctrl_sources.back();
  }
  src->setNodeAddress(node_addr);
  src->setSlot(slot);
  return src;
}


int LWRPServer::TagIsSource(const LwTag *tag) const
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


void LWRPServer::GenerateAdvertPacket(LwPacket *p,AdvertType type) const
{
  //
  // FIXME: This breaks when used with more than eight active sources!
  //
  LwTag tag;
  char hostname[33];

  switch(type) {
  case LWRPServer::Type0:
    tag.setTagName("NEST");
    tag.setTagValue(LwTag::TagType0,5+ctrl_routing->activeSources());
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(LwTag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(LwTag::TagType7,3);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(LwTag::TagType6,0x0D);  // number of bytes in following two tags
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(LwTag::TagType0,1);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(LwTag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    for(unsigned i=0;i<SWITCHYARD_SLOTS;i++) {
      if(ctrl_routing->srcEnabled(i)) {
	tag.setTagName(QString().sprintf("S%03u",i+1));
	tag.setTagValue(LwTag::TagType6,0x1C);    // number of bytes describing source
	p->addTag(tag);
	tag.setTagName("INDI");
	tag.setTagValue(LwTag::TagType0,2);
	p->addTag(tag);
	tag.setTagName("PSID");
	tag.setTagValue(LwTag::TagType1,ctrl_routing->srcAddress(i).toIPv4Address()&0xFFFF);
	p->addTag(tag);
	tag.setTagName("BUSY");
	tag.setTagValue(LwTag::TagType9,0);
	p->addTag(tag);
      }
    }
    break;

  case LWRPServer::Type1:
    tag.setTagName("NEST");
    tag.setTagValue(LwTag::TagType0,3);
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(LwTag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(LwTag::TagType7,2);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(LwTag::TagType6,0x2D);  // Length!
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(LwTag::TagType0,5);
    p->addTag(tag);
    tag.setTagName("ADVV");
    tag.setTagValue(LwTag::TagType1,0x0A);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(LwTag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    tag.setTagName("INIP");
    tag.setTagValue(LwTag::TagType1,ctrl_routing->nicAddress());
    p->addTag(tag);
    tag.setTagName("UDPC");
    tag.setTagValue(LwTag::TagType8,4000);
    p->addTag(tag);
    tag.setTagName("NUMS");
    tag.setTagValue(LwTag::TagType8,1);
    p->addTag(tag);
    break;

  case LWRPServer::Type2:
    tag.setTagName("NEST");
    tag.setTagValue(LwTag::TagType0,3+ctrl_routing->activeSources());
    p->addTag(tag);
    tag.setTagName("PVER");
    tag.setTagValue(LwTag::TagType8,2);
    p->addTag(tag);
    tag.setTagName("ADVT");
    tag.setTagValue(LwTag::TagType7,1);
    p->addTag(tag);
    tag.setTagName("TERM");
    tag.setTagValue(LwTag::TagType6,0x54);  // number of bytes to first source tag
    p->addTag(tag);
    tag.setTagName("INDI");
    tag.setTagValue(LwTag::TagType0,6);
    p->addTag(tag);
    tag.setTagName("ADVV");  // ???
    tag.setTagValue(LwTag::TagType1,0x0A);
    p->addTag(tag);
    tag.setTagName("HWID");
    tag.setTagValue(LwTag::TagType8,SWITCHYARD_HWID);
    p->addTag(tag);
    tag.setTagName("INIP");
    tag.setTagValue(LwTag::TagType1,ctrl_routing->nicAddress());
    p->addTag(tag);
    tag.setTagName("UDPC");
    tag.setTagValue(LwTag::TagType8,4000);
    p->addTag(tag);
    tag.setTagName("NUMS");
    tag.setTagValue(LwTag::TagType8,ctrl_routing->activeSources());
    p->addTag(tag);
    gethostname(hostname,32);
    tag.setTagName("ATRN");
    tag.setTagValue(LwTag::TagString,QString(hostname).split(".")[0],32); // 32 characters, zero padded
    p->addTag(tag);

    //
    // One for each source
    //
    for(unsigned i=0;i<SWITCHYARD_SLOTS;i++) {
      tag.setTagName(QString().sprintf("S%03u",i+1));
      tag.setTagValue(LwTag::TagType6,0x65);  // bytes in source record
      p->addTag(tag);
      tag.setTagName("INDI");
      tag.setTagValue(LwTag::TagType0,0x0B);
      p->addTag(tag);
      tag.setTagName("PSID");
      tag.setTagValue(LwTag::TagType1,ctrl_routing->srcNumber(i));
      p->addTag(tag);
      tag.setTagName("SHAB");
      tag.setTagValue(LwTag::TagType7,0);
      p->addTag(tag);
      tag.setTagName("FSID");
      tag.setTagValue(LwTag::TagType1,ctrl_routing->srcAddress(i));
      p->addTag(tag);
      tag.setTagName("FAST");
      tag.setTagValue(LwTag::TagType7,2);
      p->addTag(tag);
      tag.setTagName("FASM");
      tag.setTagValue(LwTag::TagType7,1);
      p->addTag(tag);
      tag.setTagName("BSID");
      tag.setTagValue(LwTag::TagType1,0);
      p->addTag(tag);
      tag.setTagName("BAST");
      tag.setTagValue(LwTag::TagType7,1);
      p->addTag(tag);
      tag.setTagName("BASM");
      tag.setTagValue(LwTag::TagType7,0);
      p->addTag(tag);
      tag.setTagName("LPID");
      tag.setTagValue(LwTag::TagType1,ctrl_routing->srcNumber(i));
      p->addTag(tag);
      tag.setTagName("STPL");
      tag.setTagValue(LwTag::TagType7,0);
      p->addTag(tag);
      tag.setTagName("PSNM");  // 16 characters, null padded
      tag.setTagValue(LwTag::TagString,ctrl_routing->srcName(i),16);
      p->addTag(tag);
    }
    break;

  case LWRPServer::TypeLast:
    break;
  }
}


int LWRPServer::GetAdvertInterval() const
{
  long base=random()/(RAND_MAX/14)+2;
  return 1000*base+10*base;
}


double LWRPServer::GetTimestamp() const
{
  struct timeval tv;
  memset(&tv,0,sizeof(tv));
  gettimeofday(&tv,NULL);
  return (double)tv.tv_sec+(double)tv.tv_usec/1000000.0;
}
