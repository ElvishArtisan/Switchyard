// syplay.cpp
//
// Switchyard stream play command
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include <QtGui/QApplication>  // So we get qApp with Qt<=4.2
#include <QCoreApplication>
#include <QTimer>

#include <sy/sycmdswitch.h>

#include "syplay.h"

bool global_exiting=false;

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
  QString filename;

  //
  // Process Command Line
  //
  SyCmdSwitch *cmd=
    new SyCmdSwitch(qApp->argc(),qApp->argv(),"syplay",VERSION,SYPLAY_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--input") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"syplay: unrecognized option\n");
      exit(256);
    }
  }

  //
  // Sanity Checks
  //
  if(filename.isEmpty()) {
    fprintf(stderr,"syplay: no output specified\n");
    exit(256);
  }

  //
  // Open Input File
  //
  SF_INFO info;
  memset(&info,0,sizeof(info));
  if((cap_sf=sf_open(filename.toAscii(),SFM_READ,&info))==NULL) {
    fprintf(stderr,"syplay: %s\n",sf_strerror(cap_sf));
    exit(256);
  }
  if(info.channels!=2) {
    fprintf(stderr,"syplay: unsupported channel count\n");
    exit(256);
  }
  if(info.samplerate!=48000) {
    fprintf(stderr,"syplay: WARNING: sample rate is %d\n",info.samplerate);
  }

  //
  // Load Routing Rules
  //
  cap_routing=new SyRouting(0,1,0,0);

  //
  // Start LWRP Server
  //
  cap_lwrp=new SyLwrpServer(cap_routing);

  //
  // Start Advertisements
  //
  cap_adv=new SyAdvServer(cap_routing,false,this);

  //
  // Start RTP
  //
  cap_rtp=new SyRtpServer(NULL,cap_sf,cap_routing,this);
  connect(cap_rtp,SIGNAL(exiting()),this,SLOT(exitData()));
  cap_rtp_hdr=new SyRtpHeader(SyRtpHeader::PayloadDynamicFirst);
  cap_rtp_hdr->setSsrc(rand());

  cap_clock=new SyClock(this);
  connect(cap_clock,SIGNAL(pllUpdated(double,int)),
	  this,SLOT(pllUpdatedData(double,int)));
  connect(cap_clock,SIGNAL(sendRtp()),this,SLOT(sendRtpData()));

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


void MainObject::pllUpdatedData(double ratio,int offset)
{
  printf("PLL Ratio: %15.13lf  Offset: %d\n",ratio,offset);
}


void MainObject::sendRtpData()
{
  char data[1500];
  int pcm[480];
  int offset;
  int n;

  //
  // Write RTP Header
  //
  (*cap_rtp_hdr)++;
  offset=4*cap_rtp_hdr->writeBlock((uint32_t *)data,1500/4);

  //
  // Write Payload
  //
  memset(pcm,0,480*sizeof(int));
  n=sf_readf_int(cap_sf,pcm,240);
  for(int i=0;i<(2*n);i++) {
    data[offset+3*i]=0xFF&(pcm[i]>>24);
    data[offset+3*i+1]=0xFF&(pcm[i]>>16);
    data[offset+3*i+2]=0xFF&(pcm[i]>>8);
  }

  cap_routing->writeRtpData(0,data,1452);
}


void MainObject::exitTimerData()
{
  if(global_exiting) {
    cap_rtp->shutdown();
  }
}


void MainObject::exitData()
{
  sf_close(cap_sf);
  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
