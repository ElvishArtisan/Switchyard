// sy5wnlogger.cpp
//
// Print WheatNet LO log messages
//
// (C) 2026 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>

#include <sy5/sycmdswitch.h>

#include "sy5wnlogger.h"

MainObject::MainObject(QObject *parent)
  : QObject()
{
  QString err_msg;
  QString proto;
  d_e2e_server=NULL;
  d_lo_server=NULL;
  d_proto=MainObject::ProtoLast;
  d_dump_body=false;

  SyCmdSwitch *cmd=new SyCmdSwitch("sy5wnlogger",VERSION,SY5WNLOGGER_USAGE);
  for(int i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--dump-body") {
      d_dump_body=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--host-address") {
      d_host_addresses.push_back(QHostAddress(cmd->value(i)));
      if(d_host_addresses.back().isNull()) {
	fprintf(stderr,
		"sy5wnlogger: invalid address \"%s\" given for --host-address",
		cmd->value(i).toUtf8().constData());
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--proto") {
      proto=cmd->value(i);
      if(proto=="lo") {
	d_proto=MainObject::ProtoLo;
      }
      if(proto=="e2e") {
	d_proto=MainObject::ProtoE2e;
      }
      if(proto=="meter") {
	d_proto=MainObject::ProtoMeter;
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"sy5wnlogger: unknown switch \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(1);
    }
  }
  if(d_proto==MainObject::ProtoLast) {
    if(proto.isEmpty()) {
      fprintf(stderr,"sy5wnlogger: you must specify a protocol\n");
    }
    else {
      fprintf(stderr,"sy5wnlogger: unknown protocol \"--proto=%s\"\n",
	      proto.toUtf8().constData());
    }
    exit(1);
  }

  switch(d_proto) {
  case MainObject::ProtoLo:
    d_lo_server=new SyLoServer(this);
    connect(d_lo_server,SIGNAL(messageReceived(SyLoMessage *)),
	    this,SLOT(messageReceivedData(SyLoMessage *)));
    if(!d_lo_server->initialize(&err_msg)) {
      fprintf(stderr,"sy5wnlogger: %s\n",err_msg.toUtf8().constData());
      exit(1);
    }
    break;

  case MainObject::ProtoE2e:
    d_e2e_server=new SyE2eServer(this);
    connect(d_e2e_server,SIGNAL(messageReceived(SyE2eMessage *)),
	    this,SLOT(messageReceivedData(SyE2eMessage *)));
    if(!d_e2e_server->initialize(&err_msg)) {
      fprintf(stderr,"sy5wnlogger: %s\n",err_msg.toUtf8().constData());
      exit(1);
    }
    break;

  case MainObject::ProtoMeter:
    d_meter_server=new SyMeterServer(this);
    connect(d_meter_server,SIGNAL(messageReceived(SyMeterMessage *)),
	    this,SLOT(messageReceivedData(SyMeterMessage *)));
    if(!d_meter_server->initialize(&err_msg)) {
      fprintf(stderr,"sy5wnlogger: %s\n",err_msg.toUtf8().constData());
      exit(1);
    }
    break;

  case MainObject::ProtoLast:
    break;
  }
}


void MainObject::messageReceivedData(SyE2eMessage *msg)
{
  if(d_host_addresses.isEmpty()||
     d_host_addresses.contains(msg->hostAddress())) {
    printf("%s",msg->dump().toUtf8().constData());
  }
  /*
    if(!d_opcodes_seen.contains(SyE2eMessage::opCodeString(msg->opCode()))) {
    printf("%s",msg->dump().toUtf8().constData());
    d_opcodes_seen.push_back(SyE2eMessage::opCodeString(msg->opCode()));
    printf("%d unique codes seen\n",d_opcodes_seen.size());
  }
  */
}


void MainObject::messageReceivedData(SyLoMessage *msg)
{
  if(d_host_addresses.isEmpty()||
     d_host_addresses.contains(msg->hostAddress())) {
    printf("%s",msg->dump().toUtf8().constData());
  }
}


void MainObject::messageReceivedData(SyMeterMessage *msg)
{
  if(d_host_addresses.isEmpty()||
     d_host_addresses.contains(msg->hostAddress())) {
    if(d_dump_body) {
      printf("%s",msg->dumpBody().toUtf8().constData());
    }
    else {
      printf("%s",msg->dump().toUtf8().constData());
    }
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  
  return a.exec();
}
