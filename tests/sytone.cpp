// sytone.cpp
//
// Switchyard tone command
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <QtGui/QApplication>  // So we get qApp with Qt<=4.2
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

#include <sy/sycmdswitch.h>

#include "sytone.h"

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
  tone_max_offset=0;
  tone_min_offset=0;

  //
  // Process Command Line
  //
  SyCmdSwitch *cmd=
    new SyCmdSwitch(qApp->argc(),qApp->argv(),"sytone",SYTONE_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"syplay: unrecognized option\n");
      exit(256);
    }
  }

  //
  // Generate Sample Buffer
  //
  int frame;
  int pcm[480];
  for(int i=0;i<240;i++) {  // Generate PCM
    frame=(int)(429496729.6*cos((double)i*0.130899695));
    pcm[2*i]=frame;
    pcm[2*i+1]=frame;
  }
  for(int i=0;i<480;i++) {  // Pack for 24 bit big-endian
    tone_buffer[12+3*i]=0xFF&(pcm[i]>>24);
    tone_buffer[12+3*i+1]=0xFF&(pcm[i]>>16);
    tone_buffer[12+3*i+2]=0xFF&(pcm[i]>>8);
  }

  //
  // Load Routing Rules
  //
  tone_routing=new SyRouting(0,1,0,0);

  //
  // Start LWRP Server
  //
  tone_lwrp=new SyLwrpServer(tone_routing);

  //
  // Start Advertisements
  //
  tone_adv=new SyAdvServer(tone_routing,false,this);

  //
  // Start RTP
  //
  tone_rtp=new SyRtpServer(NULL,NULL,tone_routing,this);
  connect(tone_rtp,SIGNAL(exiting()),this,SLOT(exitData()));
  tone_rtp_hdr=new SyRtpHeader(SyRtpHeader::PayloadDynamicFirst);
  tone_rtp_hdr->setSsrc(rand());

  //
  // Start Clock Recovery
  //
  tone_clock=new SyClock(this);
  connect(tone_clock,SIGNAL(sourceAddressChanged(const QHostAddress &)),
	  this,SLOT(sourceAddressChangedData(const QHostAddress &)));
  connect(tone_clock,SIGNAL(pllHasReset()),this,SLOT(pllHasResetData()));
  connect(tone_clock,SIGNAL(pllUpdated(double,int)),
	  this,SLOT(pllUpdatedData(double,int)));
  connect(tone_clock,SIGNAL(sendRtp()),this,SLOT(sendRtpData()));

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


void MainObject::sourceAddressChangedData(const QHostAddress &addr)
{
  printf("Clock source is now: %s\n",(const char *)addr.toString().toAscii());
}


void MainObject::pllHasResetData()
{
  printf("PLL Reset\n");
}


void MainObject::pllUpdatedData(double ratio,int offset)
{
  if(offset>tone_max_offset) {
    tone_max_offset=offset;
  }
  if(offset<tone_min_offset) {
    tone_min_offset=offset;
  }

  printf("PLL Ratio: %15.13lf  Offset: %d  Max: %d  Min: %d\n",
	 ratio,offset,tone_max_offset,tone_min_offset);
}


void MainObject::sendRtpData()
{
  int offset;

  //
  // Write RTP Header
  //
  (*tone_rtp_hdr)++;
  offset=4*tone_rtp_hdr->writeBlock((uint32_t *)tone_buffer,1500/4);

  tone_routing->writeRtpData(0,tone_buffer,1452);
}


void MainObject::exitTimerData()
{
  if(global_exiting) {
    tone_rtp->shutdown();
  }
}


void MainObject::exitData()
{
  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
