// interfacestest.cpp
//
// Test the SyInterfaces class.
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
