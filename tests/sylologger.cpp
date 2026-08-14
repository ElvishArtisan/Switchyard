// sylologger.cpp
//
// Print WheatNet LO log messages
//
// (C) 2026 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>

#include <sy5/sycmdswitch.h>

#include "sylologger.h"

MainObject::MainObject(QObject *parent)
  : QObject()
{
  QString err_msg;
  new SyCmdSwitch("sylologger",VERSION,SYLOLOGGER_USAGE);

  d_lo_server=new SyLoServer(this);
  connect(d_lo_server,SIGNAL(messageReceived(const SyLoMessage &)),
	  this,SLOT(messageReceivedData(const SyLoMessage &)));

  if(!d_lo_server->initialize(&err_msg)) {
    fprintf(stderr,"sylologger: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }
}


void MainObject::messageReceivedData(const SyLoMessage &msg)
{
  printf("%s",msg.dump().toUtf8().constData());
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  
  return a.exec();
}
