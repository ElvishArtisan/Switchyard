// syd.cpp
//
// Switchyard stream swiching daemon
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

#include <sy/sycmdswitch.h>

#include "syd.h"

bool global_exiting=false;

void *RtpCallback(unsigned dst_slot,const char *data,int len,
		  SyRouting *r,void *priv)
{
  static unsigned i;

  for(i=0;i<r->src_slots;i++) {
    if(r->src_enabled[i]&&(r->src_addr[i]!=0)&&
       (dst_slot==r->dst_addr[i])) {
      r->writeRtpData(i,data,len);
    }
  }
  return NULL;
}


void SignalHandler(int signo)
{
  switch(signo) {
  case SIGTERM:
  case SIGINT:
    global_exiting=true;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  bool debug=false;
  FILE *f=NULL;

  //
  // Process Command Line
  //
  SyCmdSwitch *cmd=new SyCmdSwitch(qApp->argc(),qApp->argv(),"syd",SYD_USAGE);
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
  syd_routing=new SyRouting(SYD_SLOTS,SYD_SLOTS,SYD_SLOTS,SYD_SLOTS);

  //
  // Start LWRP Server
  //
  syd_lwrp=new SyLwrpServer(syd_routing);

  //
  // Start Advertisements
  //
  syd_adv=new SyAdvServer(syd_routing,true,this);

  //
  // Start GPIO Processing
  //
  syd_gpio=new SyGpioServer(syd_routing,this);
  connect(syd_gpio,SIGNAL(gpiReceived(int,int,bool,bool)),
	  this,SLOT(gpiReceivedData(int,int,bool,bool)));
  connect(syd_gpio,SIGNAL(gpoReceived(int,int,bool,bool)),
	  this,SLOT(gpoReceivedData(int,int,bool,bool)));

  //
  // Detach and write PID file
  //
  if(!debug) {
    daemon(0,0);
    if((f=fopen(SYD_PID_FILE,"w"))!=NULL) {
      fprintf(f,"%u",getpid());
      fclose(f);
    }
  }

  //
  // Start RTP
  //
  syd_rtp=new SyRtpServer(RtpCallback,NULL,syd_routing,this);
  connect(syd_rtp,SIGNAL(exiting()),this,SLOT(exitData()));

  //
  // Set Signals
  //
  signal(SIGINT,SignalHandler);
  signal(SIGTERM,SignalHandler);

  //
  // Exit Timer
  //
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(exitTimerData()));
  timer->start(200);
}


void MainObject::gpiReceivedData(int gpi,int line,bool state,bool pulse)
{
  // printf("gpiReceivedData(%d,%d,%d,%d)\n",gpi,line,state,pulse);

  for(unsigned i=0;i<syd_routing->srcSlots();i++) {
    if(gpi==(int)SyRouting::livewireNumber(syd_routing->srcAddress(i))&&
       syd_routing->srcEnabled(i)) {
      syd_gpio->sendGpi(SyRouting::livewireNumber(syd_routing->dstAddress(i)),
			line,state,pulse);
    }
  }
}


void MainObject::gpoReceivedData(int gpo,int line,bool state,bool pulse)
{
  // printf("gpoReceivedData(%d,%d,%d,%d)\n",gpo,line,state,pulse);

  for(unsigned i=0;i<syd_routing->dstSlots();i++) {
    if(gpo==(int)SyRouting::livewireNumber(syd_routing->dstAddress(i))&&
       syd_routing->srcEnabled(i)) {
      syd_gpio->sendGpo(SyRouting::livewireNumber(syd_routing->srcAddress(i)),
			line,state,pulse);
    }
  }
}


void MainObject::exitTimerData()
{
  if(global_exiting) {
    syd_rtp->shutdown();
  }
}


void MainObject::exitData()
{
  syslog(LOG_DEBUG,"calling exit handler");
  unlink(SWITCHYARD_SOURCES_FILE);
  unlink(SYD_PID_FILE);
  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
