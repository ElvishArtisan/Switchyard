// sygpiotest.cpp
//
// Listen for GPIO events and print them to stdout
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <QCoreApplication>

#include "sygpiotest.h"

MainObject::MainObject(QObject *parent)
{
  gpio_routing=new SyRouting(0,0,1,1);

  gpio_server=new SyGpioServer(gpio_routing,this);
  connect(gpio_server,SIGNAL(gpioReceived(SyGpioEvent *)),
	  this,SLOT(gpioReceivedData(SyGpioEvent *)));
}


void MainObject::gpioReceivedData(SyGpioEvent *e)
{
  switch(e->type()) {
  case SyGpioEvent::TypeGpi:
    printf("GPI: ");
    break;

  case SyGpioEvent::TypeGpo:
    printf("GPO: ");
    break;

  default:
    printf("UNKNOWN: ");
    break;
  }
  printf("origin: %s:%u  ",(const char *)e->originAddress().toString().toUtf8(),
	 0xFFFF&e->originPort());
  printf("srcnum: %d  ",e->sourceNumber());
  printf("line: %d  ",e->line());
  printf("state: %d  ",e->state());
  printf("pulse: %d\n",e->isPulse());
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
