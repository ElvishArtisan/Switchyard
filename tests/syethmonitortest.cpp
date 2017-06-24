// syethmonitortest.cpp
//
// Monitor a network interface and report changes in state.
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>

#include <sy/sycmdswitch.h>

#include "syethmonitortest.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString iface;
  SyCmdSwitch *cmd=new SyCmdSwitch(qApp->argc(),qApp->argv(),"syethmonitortest",
				   VERSION,SYETHMONITORTEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
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
