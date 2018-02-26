// syclock.cpp
//
// Livewire Clock
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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
  clock_pll_interval=500;
  clock_clock_frame=0;
  clock_pcm_frame=0;
  clock_diff_clock_frame=0;
  clock_diff_pcm_frame=0;
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

  clock_watchdog_timer=new QTimer(this);
  clock_watchdog_timer->setSingleShot(true);
  connect(clock_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));

  clock_watchdog_state=false;
  clock_watchdog_timer->start(SYCLOCK_WATCHDOG_INTERVAL);
  pllData();
}


SyClock::~SyClock()
{
  delete clock_rtp_timer;
  delete clock_pll_timer;
  delete clock_watchdog_timer;
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

    if(llabs((int64_t)clock_pcm_frame-(int64_t)frame)>4800) {
      clock_clock_frame=frame;
      clock_pcm_frame=frame;
      clock_diff_setpoint=0;
      emit pllHasReset();
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
	  clock_pll_interval-=((diff-clock_diff_setpoint)/240);
	}
	if(diff<clock_diff_setpoint) {
	  clock_pll_interval+=((clock_diff_setpoint-diff)/240);
	}
	emit pllUpdated(clock_pll_interval,diff);
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
  if(clock_watchdog_state) {
    clock_watchdog_state=false;
    emit watchdogChanged(false);
  }
  clock_watchdog_timer->stop();
  clock_watchdog_timer->start(SYCLOCK_WATCHDOG_INTERVAL);
}


void SyClock::pllData()
{
  clock_burst_counter=1000;
  sendRtpData();
  clock_counter++;
  clock_pll_timer->start(clock_pll_interval);
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


void SyClock::watchdogData()
{
  clock_watchdog_state=true;
  emit watchdogChanged(true);
}
