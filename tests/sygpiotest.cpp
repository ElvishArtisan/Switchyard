// sygpiotest.cpp
//
// Listen for GPIO events and print them to stdout
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <syslog.h>

#include <QCoreApplication>

#include <sy/sycmdswitch.h>

#include "sygpiotest.h"

MainObject::MainObject(QObject *parent)
{
  gpio_log=false;
  SyCmdSwitch *cmd=new SyCmdSwitch(qApp->argc(),qApp->argv(),"sygpiotest",
				   VERSION,SYGPIOTEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--log") {
      gpio_log=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"sygpiotest: unknown option\n");
      exit(256);
    }
  }

  if(gpio_log) {
    openlog("sygpiotest",0,LOG_USER);
  }

  gpio_routing=new SyRouting(0,0,1,1);

  gpio_server=new SyGpioServer(gpio_routing,this);
  connect(gpio_server,SIGNAL(gpioReceived(SyGpioEvent *)),
	  this,SLOT(gpioReceivedData(SyGpioEvent *)));
}


void MainObject::gpioReceivedData(SyGpioEvent *e)
{
  QString msg;

  switch(e->type()) {
  case SyGpioEvent::TypeGpi:
    msg+=QString().sprintf("GPI: ");
    break;

  case SyGpioEvent::TypeGpo:
    msg+=QString().sprintf("GPO: ");
    break;

  default:
    msg+=QString().sprintf("UNKNOWN: ");
    break;
  }
  msg+="origin: "+e->originAddress().toString()+
    QString().sprintf(":%u  ",0xFFFF&e->originPort());
  msg+=QString().sprintf("srcnum: %d  ",e->sourceNumber());
  msg+=QString().sprintf("line: %d  ",e->line());
  msg+=QString().sprintf("state: %d  ",e->state());
  msg+=QString().sprintf("pulse: %d",e->isPulse());
  printf("%s\n",(const char *)msg.toUtf8());
  if(gpio_log) {
    syslog(LOG_INFO,"%s",(const char *)msg.toUtf8());
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
