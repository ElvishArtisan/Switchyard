// sycapture.cpp
//
// Switchyard stream capture command
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
#include <stdlib.h>
#include <unistd.h>

#include <QApplication>  // So we get qApp with Qt<=4.2
#include <QCoreApplication>
#include <QTimer>

#include <sy5/sycmdswitch.h>

#include "sycapture.h"

bool global_exiting=false;

void *RtpCallback(uint32_t dst_addr,const char *data,int len,
		  SyRouting *r,void *priv)
{
  static int i;
  static int pcm[480];

  for(i=12;i<len;i+=3) {
    pcm[i/3-4]=((0xFF&data[i])<<24)+((0xFF&data[i+1])<<16)+
      ((0xFF&data[i+2])<<8);
  }
  sf_writef_int((SNDFILE *)priv,pcm,len/6-2);

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
  QString filename;
  int bits=24;
  bool ok=false;

  //
  // Process Command Line
  //
  SyCmdSwitch *cmd=new SyCmdSwitch("sycapture",VERSION,SYCAPTURE_USAGE);
  for(int i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--output") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--bits") {
      bits=cmd->value(i).toInt(&ok);
      if((!ok)||((bits!=16)&&(bits!=24)&&(bits!=32))) {
	fprintf(stderr,"sycapture: invalid sample bit size specified\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"sycapture: unrecognized option\n");
      exit(256);
    }
  }

  //
  // Sanity Checks
  //
  if(filename.isEmpty()) {
    fprintf(stderr,"sycapture: no output specified\n");
    exit(256);
  }

  //
  // Open Output File
  //
  SF_INFO info;
  memset(&info,0,sizeof(info));
  info.samplerate=48000;
  info.channels=2;
  switch(bits) {
  case 16:
    info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_16;
    break;

  case 24:
    info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_24;
    break;

  case 32:
    info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_32;
    break;
  }
  if((cap_sf=sf_open(filename.toUtf8(),SFM_WRITE,&info))==NULL) {
    fprintf(stderr,"sycapture: %s\n",sf_strerror(cap_sf));
    exit(256);
  }

  //
  // Load Routing Rules
  //
  cap_routing=new SyRouting(1,0,0,0);

  //
  // Start LWRP Server
  //
  cap_lwrp=new SyLwrpServer(cap_routing);

  //
  // Start RTP
  //
  cap_rtp=new SyRtpServer(RtpCallback,cap_sf,cap_routing,this);
  connect(cap_rtp,SIGNAL(exiting()),this,SLOT(exitData()));

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
