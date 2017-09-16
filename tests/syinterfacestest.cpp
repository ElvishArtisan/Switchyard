// interfacestest.cpp
//
// Test the SyInterfaces class.
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>

#include "syinterfacestest.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  SyInterfaces *ifaces=new SyInterfaces();
  if(!ifaces->update()) {
    fprintf(stderr,"syinterfacestest: update() failed\n");
    exit(1);
  }
  for(int i=0;i<ifaces->quantity();i++) {
    printf("name: %s\n",(const char *)ifaces->name(i).toUtf8());
    if(ifaces->macAddress(i)!=0) {
      printf("MAC Address: %s\n",
	     (const char *)SyInterfaces::macString(ifaces->macAddress(i)).
	     toUtf8());
    }
    if(!ifaces->ipv4Address(i).isNull()) {
      printf("IPv4 Address: %s\n",
	     (const char *)ifaces->ipv4Address(i).toString().toUtf8());
      printf("IPv4 Netmask: %s\n",
	     (const char *)ifaces->ipv4Netmask(i).toString().toUtf8());
    }
    printf("\n");
  }
  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
