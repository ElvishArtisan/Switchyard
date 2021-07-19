// syethmonitortest.cpp
//
// Monitor a network interface and report changes in state.
//
// (C) 2017-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>

#include <sy5/sycmdswitch.h>

#include "syethmonitortest.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString iface;
  SyCmdSwitch *cmd=
    new SyCmdSwitch("syethmonitortest",VERSION,SYETHMONITORTEST_USAGE);
  for(int i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--iface") {
      iface=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"syethmonitortest: unknown argument \"%s\"\n",
	      (const char *)cmd->key(i).toUtf8());
      exit(256);
    }
  }

  test_monitor=new SyEthMonitor(iface,this);
  connect(test_monitor,SIGNAL(startedRunning()),
	  this,SLOT(startedRunningData()));
  connect(test_monitor,SIGNAL(stoppedRunning()),
	  this,SLOT(stoppedRunningData()));

  printf("Monitoring iface: %s\n",(const char *)iface.toUtf8());
}


void MainObject::startedRunningData()
{
  printf("Interface is RUNNING!\n");
}


void MainObject::stoppedRunningData()
{
  printf("Interface is NOT RUNNING!\n");
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
