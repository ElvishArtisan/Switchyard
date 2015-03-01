// syclock.cpp
//
// Livewire Clock
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include "syclock.h"
#include "syconfig.h"

SyClock::SyClock(QObject *parent)
  : QObject(parent)
{
  clock_counter=0;
  clock_burst_counter=0;
  clock_window_active=false;
  clock_pll_ratio=1.0;

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


double SyClock::pllRatio() const
{
  return clock_pll_ratio;
}


void SyClock::readyReadData()
{
  char data[1500];
  int n;
  QHostAddress addr;
  uint16_t port;
  double now;

  while((n=clock_socket->readDatagram(data,1500,&addr,&port))>0) {
    //    printf("received %d bytes from %s:%u\n",
    //	   n,(const char *)addr.toString().toAscii(),0xFFFF&port);

    now=GetTime();
    clock_tick_window.push(now);
    while(clock_tick_window.front()<(now-SYCLOCK_WINDOW_SIZE)) {
      clock_tick_window.pop();
      clock_window_active=true;
    }
    if(clock_window_active) {
      clock_pll_ratio=30.0*SYCLOCK_WINDOW_SIZE/clock_tick_window.size();
    }
  }
  //  printf("PLL Ratio: %9.7lf\n",clock_pll_ratio);
}


void SyClock::pllData()
{
  clock_burst_counter=200;
  sendRtpData();
  clock_counter++;
  clock_pll_timer->start(100.0*clock_pll_ratio);
}


void SyClock::sendRtpData()
{
  if(clock_burst_counter>0) {
    emit sendRtp();
    clock_burst_counter--;
    clock_rtp_timer->start(5);
  }
}


double SyClock::GetTime()
{
  struct timeval tv;

  memset(&tv,0,sizeof(tv));
  gettimeofday(&tv,NULL);
  return (double)tv.tv_sec+(double)tv.tv_usec/1000000.0;
}
