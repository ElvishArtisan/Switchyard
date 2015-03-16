// syclock.cpp
//
// Livewire Clock
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <arpa/inet.h>

#include "syclock.h"
#include "syconfig.h"

//
// FIXME: Rename these!
//
#define PLL_LOCKRANGE_JIFFIES	(HZ/10) /* +/-100ms */
#define PLL_TIMEOUT_JIFFIES	(10*HZ) /* 10s */

SyClock::SyClock(QObject *parent)
  : QObject(parent)
{
  clock_counter=0;
  clock_burst_counter=0;
  clock_pll_ratio=1.0;
  clock_clock_frame=0;
  clock_pcm_frame=0;
  clock_diff_clock_frame=0;
  clock_diff_pcm_frame=0;
  clock_pll_ratio=1.0;
  clock_clock_count=0;

  //
  // Clock Socket
  //
  clock_socket=new SyMcastSocket(SyMcastSocket::ReadOnly,this);
  clock_socket->bind(SWITCHYARD_CLOCK_PORT);
  clock_socket->subscribe(SWITCHYARD_CLOCK_ADDRESS);
  connect(clock_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  //
  // Timers
  //
  clock_pll_timer=new QTimer(this);
  clock_pll_timer->setSingleShot(true);
  connect(clock_pll_timer,SIGNAL(timeout()),this,SLOT(pllData()));

  clock_rtp_timer=new QTimer(this);
  clock_rtp_timer->setSingleShot(true);
  connect(clock_rtp_timer,SIGNAL(timeout()),this,SLOT(sendRtpData()));

  pllData();
}


SyClock::~SyClock()
{
  delete clock_rtp_timer;
  delete clock_pll_timer;
  delete clock_socket;
}


QHostAddress SyClock::sourceAddress() const
{
  return clock_source_address;
}


void SyClock::readyReadData()
{
  char data[1500];
  int n;
  QHostAddress addr;
  uint16_t port;
  uint32_t frame;

  while((n=clock_socket->readDatagram(data,1500,&addr,&port))>0) {
    //seq=ntohl(*((uint32_t *)data));
    frame=ntohl(*((uint32_t *)(data+4)));

    if(clock_clock_frame==0) {  // Initialize PLL
      clock_clock_frame=frame;
      clock_pcm_frame=frame;
      clock_diff_setpoint=0;
    }
    else {
      if((clock_clock_count==0)||
	 ((frame-clock_pcm_frame)<
	  (clock_diff_clock_frame-clock_diff_pcm_frame))) {
	clock_diff_clock_frame=frame;
	clock_diff_pcm_frame=clock_pcm_frame;
      }
      if(clock_clock_count++>=50) {
	int64_t diff=(int64_t)clock_diff_clock_frame-
	  (int64_t)clock_diff_pcm_frame;
	if(diff>clock_diff_setpoint) {
	  clock_pll_ratio=clock_pll_ratio*0.9999999999;
	}
	if(diff<clock_diff_setpoint) {
	  clock_pll_ratio=clock_pll_ratio*1.0000000001;
	}
	emit pllUpdated(clock_pll_ratio,diff);
	/*
	printf("clock: %u  pcm: %u  diff: %ld  ratio: %15.13lf\n",
	       clock_diff_clock_frame,clock_diff_pcm_frame,diff,clock_pll_ratio);
	*/
	clock_diff_setpoint=diff;
	clock_clock_count=0;
      }
    }
    if(addr!=clock_source_address) {
      clock_source_address=addr;
      emit sourceAddressChanged(clock_source_address);
    }
    //    printf("SEQ: %u  FRAME: %u\n",seq,frame);
  }
}


void SyClock::pllData()
{
  clock_burst_counter=1000;
  sendRtpData();
  clock_counter++;
  clock_pll_timer->start(500.0*clock_pll_ratio);
}


void SyClock::sendRtpData()
{
  if(clock_burst_counter>0) {
    emit sendRtp();
    clock_burst_counter--;
    clock_pcm_frame+=240;
    clock_rtp_timer->start(5);
  }
}
