// sytone.cpp
//
// Switchyard tone command
//
// (C) 2014-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <signal.h>
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

#include <QApplication>
#include <QCoreApplication>
#include <QTimer>

#include <sy5/sycmdswitch.h>

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
  SyCmdSwitch *cmd=new SyCmdSwitch("sytone",VERSION,SYTONE_USAGE);
  for(int i=0;i<cmd->keys();i++) {
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
  // Get Realtime Permissions
  //
  struct sched_param sched;
  memset(&sched,0,sizeof(sched));
  sched.sched_priority=1;
  if(sched_setscheduler(0,SCHED_FIFO,&sched)!=0) {
    fprintf(stderr,"sytone: unable to set realtime priority\n");
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
  connect(tone_clock,SIGNAL(pllUpdated(int,int)),
	  this,SLOT(pllUpdatedData(int,int)));
  connect(tone_clock,SIGNAL(watchdogChanged(bool)),
	  this,SLOT(clockWatchdogChangedData(bool)));
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
  printf("Clock source is now: %s\n",addr.toString().toUtf8().constData());
}


void MainObject::pllHasResetData()
{
  printf("PLL Reset\n");
}


void MainObject::clockWatchdogChangedData(bool state)
{
  printf("Clock watchdog barked: %d\n",state);
}


void MainObject::pllUpdatedData(int interval,int offset)
{
  if(offset>tone_max_offset) {
    tone_max_offset=offset;
  }
  if(offset<tone_min_offset) {
    tone_min_offset=offset;
  }

  printf("PLL Interval: %d  Offset: %d  Max: %d  Min: %d\n",
	 interval,offset,tone_max_offset,tone_min_offset);
}


void MainObject::sendRtpData()
{
  //
  // Write RTP Header
  //
  (*tone_rtp_hdr)++;
  tone_rtp_hdr->writeBlock((uint32_t *)tone_buffer,1500/4);

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
