// sy5advpeek.cpp
//
// Utility for monitoring Livewire advertising packets
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

#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>

#include <sy5/sycmdswitch.h>

#include "sy5advpeek.h"

MainObject::MainObject()
  : QObject()
{
  d_advertisments=false;
  d_reservations=false;

  //
  // Read CLI Switches
  //
  SyCmdSwitch *cmd=new SyCmdSwitch("sy5advpeek",VERSION,SY5ADVPEEK_USAGE);
  for(int i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--advertisments") {
      d_advertisments=true;
      QStringList addrs=cmd->value(i).split(",");
      if(!cmd->value(i).isEmpty()) {
	for(int i=0;i<addrs.size();i++) {
	  QHostAddress addr;
	  if(addr.setAddress(addrs.at(i))) {
	    d_advert_addrs.push_back(addr);
	  }
	  else {
	    fprintf(stderr,"advertisement source address \"%s\" is invalid\n",
		    addrs.at(i).toUtf8().constData());
	    exit(1);
	  }
	}
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--reservations") {
      d_reservations=true;
      QStringList addrs=cmd->value(i).split(",");
      if(!cmd->value(i).isEmpty()) {
	for(int i=0;i<addrs.size();i++) {
	  QHostAddress addr;
	  if(addr.setAddress(addrs.at(i))) {
	    d_reserve_addrs.push_back(addr);
	  }
	  else {
	    fprintf(stderr,"reservation source address \"%s\" is invalid\n",
		    addrs.at(i).toUtf8().constData());
	    exit(1);
	  }
	}
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"unrecognized switch \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(1);
    }
  }

  //
  // Create the Routing Object
  //
  d_routing=new SyRouting(0,0,0,0);

  //
  // Start the Advertising Server
  //
  d_server=new SyAdvServer(d_routing,true,this);
  if(d_advertisments) {
    connect(d_server,
	    SIGNAL(advertismentReceived(const QHostAddress &,
					const SyAdvPacket &)),
	    this,
	    SLOT(advertismentReceivedData(const QHostAddress &,
					  const SyAdvPacket &)));
  }
  if(d_reservations) {
    connect(d_server,
	    SIGNAL(reservationReceived(const QHostAddress &,
				       const SyAdvPacket &)),
	    this,
	    SLOT(reservationReceivedData(const QHostAddress &,
					 const SyAdvPacket &)));
  }
}


void MainObject::advertismentReceivedData(const QHostAddress &addr,
					  const SyAdvPacket &advert)
{
  if((d_advert_addrs.size()==0)||ContainsV4Address(d_advert_addrs,addr)) {
    printf("%s\n",MakeTimestamp("ADVERTISMENT",addr).toUtf8().constData());
    printf("%s\n",advert.dump().toUtf8().constData());
    printf("%s\n",MakeFooter().toUtf8().constData());
  }
}


void MainObject::reservationReceivedData(const QHostAddress &addr,
					 const SyAdvPacket &reserv)
{
  printf("%s\n",MakeTimestamp("RESERVATION",addr).toUtf8().constData());
  printf("%s\n",reserv.dump().toUtf8().constData());
  printf("%s\n",MakeFooter().toUtf8().constData());
}


QString MainObject::MakeTimestamp(const QString &type,
				  const QHostAddress &addr) const
{
  QString straddr=addr.toString();
  straddr.replace("::ffff:","");
  QString ret;

  QTime now=QTime::currentTime();
  QString ms=QString::asprintf("%03d",now.msec());
  ret=QTime::currentTime().toString("hh:mm:ss")+"."+ms;
  ret+=" "+type+" from "+straddr;
  
  return ret;
}


QString MainObject::MakeFooter() const
{
  return QString("*****************************************************************************");
}


bool MainObject::ContainsV4Address(const QList<QHostAddress> &addrs,
				   const QHostAddress &addr) const
{
  for(int i=0;i<addrs.size();i++) {
    if(addrs.at(i).isEqual(addr,QHostAddress::TolerantConversion)) {
      return true;
    }
  }
  return false;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();

  return a.exec();
}
