// sylwrp_server.cpp
//
// Livewire Control Protocol Server
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "sysyslog.h"
#include "sylwrp_server.h"

SyLwrpServer::SyLwrpServer(SyRouting *routing)
  : QObject()
{
  QHostAddress addr;

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
    SySyslog(LOG_ERR,QString().
	     sprintf("unable to bind port %d",SWITCHYARD_LWRP_PORT));
    exit(256);
  }

  //
  // Initialize slots
  //
  for(unsigned i=0;i<ctrl_routing->srcSlots();i++) {
    ctrl_routing->subscribe(ctrl_routing->dstAddress(i));
  }
}


void SyLwrpServer::sendGpiState(int slot,const QString &code)
{
  SyLwrpClientConnection *conn=NULL;

  for(int i=0;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
    bool state=code.mid(i,1).toLower()=="l";
    ctrl_routing->setGpiBySlot(slot,i,state,false);
  }

  for(unsigned i=0;i<ctrl_client_connections.size();i++) {
    if((conn=ctrl_client_connections.at(i))!=NULL) {
      if(conn->gpiAdded(slot)) {
	SendCommand(i,QString().sprintf("GPI %d ",slot+1)+code);
      }
    }
  }
}


void SyLwrpServer::sendGpoState(int slot,const QString &code)
{
  SyLwrpClientConnection *conn=NULL;

  for(int i=0;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
    bool state=code.mid(i,1).toLower()=="l";
    ctrl_routing->setGpoBySlot(slot,i,state,false);
  }

  for(unsigned i=0;i<ctrl_client_connections.size();i++) {
    if((conn=ctrl_client_connections.at(i))!=NULL) {
      if(conn->gpoAdded(slot)) {
	SendCommand(i,QString().sprintf("GPO %d ",slot+1)+code);
      }
    }
  }
}


void SyLwrpServer::newConnectionData()
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
  ctrl_client_connections[id]=new SyLwrpClientConnection();
  ctrl_client_connections[id]->setSocket(ctrl_server->nextPendingConnection());
  ctrl_read_mapper->setMapping (ctrl_client_connections[id]->socket(),id);
  connect(ctrl_client_connections[id]->socket(),SIGNAL(readyRead()),
	  ctrl_read_mapper,SLOT(map()));
  ctrl_closed_mapper->setMapping (ctrl_client_connections[id]->socket(),id);
  connect(ctrl_client_connections[id]->socket(),SIGNAL(connectionClosed()),
	  ctrl_closed_mapper,SLOT(map()));
}


void SyLwrpServer::readData(int id)
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


void SyLwrpServer::closedData(int id)
{
  delete ctrl_client_connections[id];
  ctrl_client_connections[id]=NULL;
}


bool SyLwrpServer::ExecuteLogin(int id,QStringList &args)
{
  return true;
}


bool SyLwrpServer::ExecuteVer(int id,QStringList &args)
{
  SendCommand(id,QString().
    sprintf("VER LWRP:%s DEVN:\"%s\" SYSV:%s NSRC:%u/2 NDST:%u NGPI:%u NGPO:%u",
	     SWITCHYARD_LWRP_VERSION,SWITCHYARD_DEVICE_NAME,VERSION,
	     ctrl_routing->srcSlots(),ctrl_routing->dstSlots(),
	     ctrl_routing->gpis(),ctrl_routing->gpos()));
  return true;
}


bool SyLwrpServer::ExecuteIp(int ch,QStringList &args)
{
  SendCommand(ch,QString("IP address ")+
	      ctrl_routing->nicAddress().toString()+" "+
	      "netmask "+ctrl_routing->nicNetmask().toString()+" "+
	      "gateway 0.0.0.0 "+
	      "hostname "+ctrl_routing->hostName());

  return true;
}


bool SyLwrpServer::ExecuteSrc(int id,QStringList &args)
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
    for(unsigned i=0;i<ctrl_routing->srcSlots();i++) {
      SendCommand(id,SrcLine(i));
    }
    SendCommand(id,"END");
  }
  else {
    slot=args[1].toInt(&ok)-1;
    if((!ok)|(slot<0)||(slot>=(int)ctrl_routing->srcSlots())) {
      return false;
    }
    for(int i=2;i<args.size();i++) {
      found=false;
      fields=ParseField(SyAString(args[i]));
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
      if(fields[0]=="FASM") {
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


bool SyLwrpServer::ExecuteDst(int id,QStringList &args)
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
    for(unsigned i=0;i<ctrl_routing->dstSlots();i++) {
      SendCommand(id,DstLine(i));
    }
    SendCommand(id,"END");
  }
  else {
    slot=args[1].toInt(&ok)-1;
    if((!ok)|(slot<0)||(slot>=(int)ctrl_routing->dstSlots())) {
      return false;
    }
    for(int i=2;i<args.size();i++) {
      found=false;
      fields=ParseField(SyAString(args[i]));
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


bool SyLwrpServer::ExecuteGpi(int ch,QStringList &args)
{
  unsigned slot;
  bool ok=false;

  switch(args.size()) {
  case 1:
    SendCommand(ch,"BEGIN");
    for(unsigned i=0;i<ctrl_routing->gpis();i++) {
      SendCommand(ch,GpiLine(i));
    }
    SendCommand(ch,"END");
    return true;

  case 2:
    slot=args[1].toUInt(&ok);
    if(ok&&(slot<=ctrl_routing->gpis())) {
      SendCommand(ch,GpiLine(slot-1));
      return true;
    }
    break;
  }
  return false;
}


bool SyLwrpServer::ExecuteGpo(int ch,QStringList &args)
{
  unsigned slot;
  bool ok=false;

  switch(args.size()) {
  case 1:
    SendCommand(ch,"BEGIN");
    for(unsigned i=0;i<ctrl_routing->gpos();i++) {
      SendCommand(ch,GpoLine(i));
    }
    SendCommand(ch,"END");
    return true;

  case 2:
    slot=args[1].toUInt(&ok);
    if(ok&&(slot<=ctrl_routing->gpos())) {
      SendCommand(ch,GpoLine(slot-1));
      return true;
    }
    break;
  }
  return false;
}


bool SyLwrpServer::ExecuteIfc(int ch,QStringList &args)
{
  if(args.size()>2) {
    return false;
  }
  if(args.size()==2) {
    QHostAddress addr(args[1]);
    if(addr.isNull()) {
      return false;
    }
    ctrl_routing->setNicAddress(addr);
  }
  SendCommand(ch,"IFC "+ctrl_routing->nicAddress().toString());

  return true;
}


bool SyLwrpServer::ExecuteCfg(int ch,QStringList &args)
{
  unsigned slot=0;
  unsigned srcnum=0;
  QStringList fields;
  QString addr="0.0.0.0";
  bool save=false;
  bool ok=false;

  if(args.size()<2) {
    return false;
  }
  if(args[1]!="GPO") {
    return false;
  }
  if(args.size()==2) {
    SendCommand(ch,"BEGIN");
    for(unsigned i=0;i<ctrl_routing->gpos();i++) {
      SendCommand(ch,CfgLine(i));
    }
    SendCommand(ch,"END");
    return true;
  }
  if(args.size()>=3) {
    slot=args[2].toInt(&ok)-1;
    if((!ok)||(slot>=ctrl_routing->gpos())) {
      return false;
    }
  }
  if(args.size()==3) {
    SendCommand(ch,CfgLine(slot));
    return true;
  }
  for(int i=3;i<args.size();i++) {
    fields=ParseField(SyAString(args[i]));
    if(fields[0]=="NAME") {
      if(fields.size()>1) {
	ctrl_routing->setGpoName(slot,fields[1]);
	save=true;
      }
    }
    if(fields[0]=="SRCA") {
      if(fields.size()>1) {
	srcnum=fields[1].toUInt(&ok);
	if(ok&&(srcnum<32768)) {
	  ctrl_routing->setGpoAddress(slot,
		       SyRouting::streamAddress(SyRouting::Stereo,srcnum));
	  ctrl_routing->setGpoMode(slot,SyRouting::GpoFollowSource);
	  save=true;
	}
	else {
	  QHostAddress addr(fields[1]);
	  if(!addr.isNull()) {
	    ctrl_routing->setGpoAddress(slot,addr);
	    ctrl_routing->setGpoMode(slot,SyRouting::GpoFollowSource);
	    save=true;
	  }
	  else {
	    QStringList f0=fields[1].split("/");
	    if(f0.size()==2) {
	      addr.setAddress(f0[0]);
	      int snake_slot=f0[1].toInt(&ok)-1;
	      if((!addr.isNull())&&ok) {
		ctrl_routing->setGpoMode(slot,SyRouting::GpoSnake);
		ctrl_routing->setGpoAddress(slot,addr);
		ctrl_routing->setGpoSnakeSlot(slot,snake_slot);
		save=true;
	      }
	      else {
		return false;
	      }
	    }
	    else {
	      ctrl_routing->setGpoMode(slot,SyRouting::GpoFollowSource);
	      ctrl_routing->setGpoAddress(slot,QHostAddress());
	      ctrl_routing->setGpoSnakeSlot(slot,0);
	      save=true;
	    }
	  }
	}
      }
      else {
	ctrl_routing->setGpoAddress(slot,"0.0.0.0");
      }
    }
  }

  if(save) {
    ctrl_routing->save();
    emit gpoCfgChanged(slot,ctrl_routing->gpoMode(slot),
		       ctrl_routing->gpoAddress(slot),
    		       ctrl_routing->gpoSnakeSlot(slot));
  }
  SendCommand(ch,CfgLine(slot));
  
  return true;
}


bool SyLwrpServer::ExecuteAdd(int ch,QStringList &args)
{
  unsigned slot;
  bool ok=false;
  bool ret=false;

  /*
  if(args.size()!=2) {
    return ret;
  }
  */

  if(args[1]=="GPI") {
    if(args.size()==2) {
      for(unsigned i=0;i<ctrl_routing->gpis();i++) {
	ctrl_client_connections[ch]->gpiAdd(i);
      }
      SendCommand(ch,"BEGIN");
      for(unsigned i=0;i<ctrl_routing->gpis();i++) {
	SendCommand(ch,GpiLine(i));
      }
      SendCommand(ch,"END");
      ret=true;
    }
    if(args.size()==3) {
      slot=args[2].toUInt(&ok)-1;
      if(ok&&(slot<ctrl_routing->gpis())) {
	ctrl_client_connections[ch]->gpiAdd(slot);
	SendCommand(ch,GpiLine(slot));
	ret=true;
      }
    }
  }
  if(args[1]=="GPO") {
    if(args.size()==2) {
      for(unsigned i=0;i<ctrl_routing->gpos();i++) {
	ctrl_client_connections[ch]->gpoAdd(i);
      }
      SendCommand(ch,"BEGIN");
      for(unsigned i=0;i<ctrl_routing->gpos();i++) {
	SendCommand(ch,GpoLine(i));
      }
      SendCommand(ch,"END");
      ret=true;
    }
    if(args.size()==3) {
      slot=args[2].toUInt(&ok)-1;
      if(ok&&(slot<ctrl_routing->gpos())) {
	ctrl_client_connections[ch]->gpoAdd(slot);
	SendCommand(ch,GpoLine(slot));
	ret=true;
      }
    }
  }

  return ret;
}


bool SyLwrpServer::ExecuteDel(int ch,QStringList &args)
{
  unsigned slot;
  bool ok=false;
  bool ret=false;

  /*
  if(args.size()!=2) {
    return ret;
  }
  */

  if(args[1]=="GPI") {
    if(args.size()==2) {
      for(unsigned i=0;i<ctrl_routing->gpis();i++) {
	ctrl_client_connections[ch]->gpiDel(i);
      }
      ret=true;
    }
    if(args.size()==3) {
      slot=args[2].toUInt(&ok)-1;
      if(ok&&(slot<ctrl_routing->gpis())) {
	ctrl_client_connections[ch]->gpiDel(slot);
	ret=true;
      }
    }
  }
  if(args[1]=="GPO") {
    if(args.size()==2) {
      for(unsigned i=0;i<ctrl_routing->gpos();i++) {
	ctrl_client_connections[ch]->gpoDel(i);
      }
      ret=true;
    }
    if(args.size()==3) {
      slot=args[2].toUInt(&ok)-1;
      if(ok&&(slot<ctrl_routing->gpos())) {
	ctrl_client_connections[ch]->gpoDel(slot);
	ret=true;
      }
    }
  }

  return ret;
}


QString SyLwrpServer::SrcLine(int slot)
{
  return QString().
    sprintf("SRC %u PSNM:\"%s\" FASM:1 RTPE:%d RTPA:\"%s\" INGN:0 SHAB:0 NCHN:2 RTPP:240",
	    slot+1,
	    (const char *)ctrl_routing->srcName(slot).toAscii(),
	    ctrl_routing->srcEnabled(slot),
	    (const char *)ctrl_routing->srcAddress(slot).toString().toAscii());
}


QString SyLwrpServer::DstLine(int slot)
{
  return QString().
    sprintf("DST %u NAME:\"%s\" ADDR:\"%s\" NCHN:2 LOAD:0 OUGN:0",
	    slot+1,
	    (const char *)ctrl_routing->dstName(slot).toAscii(),
	    (const char *)ctrl_routing->dstAddress(slot).toString().toAscii());
}


QString SyLwrpServer::GpiLine(int slot)
{
  QString ret=QString().sprintf("GPI %d ",slot+1);

  for(int i=0;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
    if(ctrl_routing->gpiStateBySlot(slot,i)) {
      ret+="l";
    }
    else {
      ret+="h";
    }
  }

  return ret;
}


QString SyLwrpServer::GpoLine(int slot)
{
  QString ret=QString().sprintf("GPO %d ",slot+1);

  for(int i=0;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
    if(ctrl_routing->gpoStateBySlot(slot,i)) {
      ret+="l";
    }
    else {
      ret+="h";
    }
  }

  return ret;
}


QString SyLwrpServer::CfgLine(int slot)
{
  QString ret=QString().sprintf("CFG GPO %d",slot+1);

  ret+=" NAME:\""+ctrl_routing->gpoName(slot)+"\"";
  bool srca_sent=false;
  if(ctrl_routing->gpoMode(slot)==SyRouting::GpoFollowSource) {
    if(SyRouting::livewireNumber(ctrl_routing->gpoAddress(slot))!=0) {
      ret+=QString().sprintf(" SRCA:\"%u\"",
			     SyRouting::livewireNumber(ctrl_routing->gpoAddress(slot)));
      srca_sent=true;
    }
  }
  if(ctrl_routing->gpoMode(slot)==SyRouting::GpoSnake) {
    if(!ctrl_routing->gpoAddress(slot).isNull()) {
      ret+=QString(" SRCA:")+ctrl_routing->gpoAddress(slot).toString()+
	QString().sprintf("/%d",ctrl_routing->gpoSnakeSlot(slot)+1);
      srca_sent=true;
    }
  }
  if(!srca_sent) {
    ret+=" SRCA:";
  }

  return ret;
}


QStringList SyLwrpServer::ParseField(const SyAString &field)
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


void SyLwrpServer::ParseCommand(int id)
{
  bool ok=false;
  QStringList args=
    SyAString(ctrl_client_connections[id]->commandBuffer()).split(" ","\"");

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
  if(args[0]=="gpi") {
    ok=ExecuteGpi(id,args);
  }
  if(args[0]=="gpo") {
    ok=ExecuteGpo(id,args);
  }
  if(args[0]=="ifc") {
    ok=ExecuteIfc(id,args);
  }
  if(args[0]=="cfg") {
    ok=ExecuteCfg(id,args);
  }
  if(args[0]=="add") {
    ok=ExecuteAdd(id,args);
  }
  if(args[0]=="del") {
    ok=ExecuteDel(id,args);
  }
  if(!ok) {
    SendCommand(id,"ERROR 1000 bad command");
  }
}


void SyLwrpServer::BroadcastCommand(const QString &cmd)
{
  for(unsigned i=0;i<ctrl_client_connections.size();i++) {
    if(ctrl_client_connections[i]!=NULL) {
      SendCommand(i,cmd);
    }
  }
}


void SyLwrpServer::SendCommand(int ch,const QString &cmd)
{
  ctrl_client_connections[ch]->
    socket()->write((cmd+"\r\n").toAscii(),cmd.length()+2);
}


void SyLwrpServer::UnsubscribeStream(int slot)
{
  if(ctrl_routing->clkAddress()==ctrl_routing->dstAddress(slot)) {
    return;
  }
  for(unsigned i=0;i<ctrl_routing->srcSlots();i++) {
    if((ctrl_routing->dstAddress(i)==ctrl_routing->dstAddress(slot))&&
       ((int)i!=slot)) {
      return;
    }
  }
  ctrl_routing->unsubscribe(ctrl_routing->dstAddress(slot));
}
