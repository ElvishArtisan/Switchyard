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
  connect(gpio_server,SIGNAL(gpiReceived(int,int,bool,bool)),
	  this,SLOT(gpiReceivedData(int,int,bool,bool)));
  connect(gpio_server,SIGNAL(gpoReceived(int,int,bool,bool)),
	  this,SLOT(gpoReceivedData(int,int,bool,bool)));
}


void MainObject::gpiReceivedData(int gpi,int line,bool state,bool pulse)
{
  printf("GPI: gpi: %d  line: %d  state: %d  pulse: %u\n",
	 gpi,line,state,pulse);
}


void MainObject::gpoReceivedData(int gpo,int line,bool state,bool pulse)
{
  printf("GPO: gpo: %d  line: %d  state: %d  pulse: %u\n",
	 gpo,line,state,pulse);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
