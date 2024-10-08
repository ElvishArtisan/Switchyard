// sygpiotest.cpp
//
// Listen for GPIO events and print them to stdout
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>

#include <QCoreApplication>

#include <sy/sycmdswitch.h>

#include "sygpiotest.h"

MainObject::MainObject(QObject *parent)
{
  QHostAddress iface_address;
  gpio_log=false;
  SyCmdSwitch *cmd=new SyCmdSwitch(qApp->argc(),qApp->argv(),"sygpiotest",
				   VERSION,SYGPIOTEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--iface-address") {
      if(!iface_address.setAddress(cmd->value(i))) {
	fprintf(stderr,"sygpiotest: invalid \"--iface-address\" specified\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
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
  if(!iface_address.isNull()) {
    gpio_routing->setNicAddress(iface_address);
  }

  gpio_server=new SyGpioServer(gpio_routing,this);
  connect(gpio_server,SIGNAL(gpioReceived(SyGpioEvent *)),
	  this,SLOT(gpioReceivedData(SyGpioEvent *)));
}


void MainObject::gpioReceivedData(SyGpioEvent *e)
{
  QString msg;

  msg+=SyGpioEvent::gpioTypeString(e->type())+" ";
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
