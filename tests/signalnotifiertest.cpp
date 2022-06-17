// signalnotifiertest.cpp
//
// Test the SySignalNotifier class.
//
// (C) 2022 Fred Gleason <fredg@paravelsystems.com>
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
#include <sys/types.h>

#include <QCoreApplication>

#include <sy5/sycmdswitch.h>

#include "signalnotifiertest.h"

MainObject::MainObject()
  : QObject()
{
  //
  // Read Arguments
  //
  bool ok;
  SyCmdSwitch *cmd=
    new SyCmdSwitch("signalnotifiertest",VERSION,SIGNALNOTIFIERTEST_USAGE);

  for(int i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--signum") {
      d_signals.push_back(cmd->value(i).toInt(&ok));
      if((!ok)||(d_signals.back()<=0)||(d_signals.back()>=SIGSYS)) {
	fprintf(stderr,"signalnotifiertest: invalid signal number \"%s\"\n",
		cmd->value(i).toUtf8().constData());
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"signalnotifiertest: unrecognized option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(1);
    }
  }

  //
  // Run the test
  //
  d_signal_notifier=new SySignalNotifier(this);
  connect(d_signal_notifier,SIGNAL(activated(int)),
	  this,SLOT(activatedData(int)));
  for(int i=0;i<d_signals.size();i++) {
    d_signal_notifier->addSignal(d_signals.at(i));
  }
  
  printf("PROCESS %d READY...\n",getpid());
}


void MainObject::activatedData(int signum)
{
  printf("received signal: %d\n",signum);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();

  return a.exec();
}
