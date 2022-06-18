// sysignalnotifier.cpp
//
// Notifications for UNIX-style signals
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "sysignalnotifier.h"

int __sysignalnotifier_ipc_sockets[2];

static void __SySignalNotifier_SignalHandler(int signo)
{
  if(write(__sysignalnotifier_ipc_sockets[0],&signo,sizeof(int))) {
    // Do nothing, the conditional is for suppressing a compilation warning
  }  
}


SySignalNotifier::SySignalNotifier(QObject *parent)
  : QObject(parent)
{
  //
  // Initialize IPC
  //
  __sysignalnotifier_ipc_sockets[0]=-1;
  __sysignalnotifier_ipc_sockets[1]=-1;
  if(socketpair(AF_UNIX,SOCK_STREAM,0,__sysignalnotifier_ipc_sockets)<0) {
    fprintf(stderr,"SySignalNotifier: unable to initialize IPC [%s]\n",
	    strerror(errno));
    exit(1);
  }
  d_ipc_notifier=new QSocketNotifier(__sysignalnotifier_ipc_sockets[1],
				     QSocketNotifier::Read,this);
  //  connect(d_ipc_notifier,
  //	  SIGNAL(activated(QSocketDescriptor,QSocketNotifier::Type)),
  //	  this,
  //	  SLOT(socketActivatedData(QSocketDescriptor,QSocketNotifier::Type)));
  connect(d_ipc_notifier,SIGNAL(activated(int)),
	  this,SLOT(socketActivatedData(int)));
}


SySignalNotifier::~SySignalNotifier()
{
  if(shutdown(__sysignalnotifier_ipc_sockets[1],SHUT_RDWR)<0) {
    fprintf(stderr,"SySignalNotifier: IPC shutdown failed [%s]\n",
	    strerror(errno));
  }
  delete d_ipc_notifier;
}


QList<int> SySignalNotifier::monitoredSignals() const
{
  return d_signal_numbers;
}


bool SySignalNotifier::addSignal(int signum)
{
  struct sigaction sa;

  if(!d_signal_numbers.contains(signum)) {
    memset(&sa,0,sizeof(struct sigaction));
    sa.sa_handler=__SySignalNotifier_SignalHandler;
    if(sigaction(signum,&sa,NULL)<0) {
      fprintf(stderr,"SySignalNotifier: unable to add signal %d [%s]\n",
	      signum,strerror(errno));
      return false;
    }
  }
  d_signal_numbers.push_back(signum);

  return true;
}


bool SySignalNotifier::removeSignal(int signum)
{
  struct sigaction sa;

  if(d_signal_numbers.contains(signum)) {
    memset(&sa,0,sizeof(struct sigaction));
    sa.sa_handler=SIG_DFL;
    if(sigaction(signum,&sa,NULL)<0) {
      fprintf(stderr,"SySignalNotifier: unable to remove signal %d [%s]\n",
	      signum,strerror(errno));
    }
    d_signal_numbers.removeAll(signum);
  }

  return true;
}


//void SySignalNotifier::socketActivatedData(QSocketDescriptor sock,
//					   QSocketNotifier::Type type)
void SySignalNotifier::socketActivatedData(int sock)
{
  int signum;
  int n;

  if((n=read(__sysignalnotifier_ipc_sockets[1],&signum,sizeof(int)))!=
     sizeof(int)) {
    if(n<0) {
      fprintf(stderr,"SySignalNotifier: error reading IPC [%s]\n",
	      strerror(errno));
    }
    else {
      fprintf(stderr,"SySignalNotifier: unexpected IPC data size [%d/%ld]\n",
	      n,sizeof(int));
    }
    return;
  }
  if(!d_signal_numbers.contains(signum)) {
    fprintf(stderr,
       "SySignalNotifier: received notification from non-installed signal %d\n",
	signum);
    return;
  }

  emit activated(signum);
}
