// syethmonitor.cpp
//
// Monitor a physical Ethernet interface
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "syethmonitor.h"

SyEthMonitor::SyEthMonitor(const QString &iface,QObject *parent)
  : QObject(parent)
{
  eth_interface_name=iface;
  eth_is_running=false;

  eth_timer=new QTimer(this);
  connect(eth_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));

  if((eth_socket=socket(AF_INET,SOCK_DGRAM,0))>=0) {
    eth_timer->start(SYETHMONITOR_MONITOR_INTERVAL);
  }
  else {
    fprintf(stderr,"SyEthMonitor: unable to create monitor socket [%s]\n",
	    strerror(errno));
  }
}


SyEthMonitor::~SyEthMonitor()
{
  close(eth_socket);
  delete eth_timer;
}


QString SyEthMonitor::interfaceName() const
{
  return eth_interface_name;
}


bool SyEthMonitor::isRunning() const
{
  return eth_is_running;
}


void SyEthMonitor::timeoutData()
{
  memset(&eth_ifreq,0,sizeof(eth_ifreq));
  strncpy(eth_ifreq.ifr_name,eth_interface_name.toUtf8(),IFNAMSIZ);
  if(ioctl(eth_socket,SIOCGIFFLAGS,&eth_ifreq)<0) {
    eth_timer->stop();
    fprintf(stderr,"SyEthMonitor: ioctl returned error [%s]\n",
	    strerror(errno));
    return;
  }
  
  if((eth_ifreq.ifr_flags&IFF_RUNNING)==0) {
    if(eth_is_running) {
      eth_is_running=false;
      emit stoppedRunning();
    }
  }
  else {
    if(!eth_is_running) {
      eth_is_running=true;
      emit startedRunning();
    }
  }
}
