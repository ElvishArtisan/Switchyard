// syd.cpp
//
// Switchyard stream swiching daemon
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include <QtCore/QCoreApplication>

#include "cmdswitch.h"

#include "syd.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  bool debug=false;

  //
  // Process Command Line
  //
  CmdSwitch *cmd=new CmdSwitch(qApp->argc(),qApp->argv(),"syd",SYD_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="-d") {
      debug=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"syd: unrecognized option\n");
      exit(256);
    }
  }

  //
  // Open Syslog
  //
  openlog("syd",LOG_NDELAY|LOG_PERROR,LOG_DAEMON);

  //
  // Load Routing Rules
  //
  syd_routing=new Routing();

  //
  // Start LWRP Server
  //
  syd_lwrp=new LWRPServer(syd_routing);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
