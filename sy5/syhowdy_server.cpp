// syhowdy_server.cpp
//
// WheatNet HOWDY Logging Server
//
// (C) Copyright 2026 Fred Gleason <fredg@paravelsystems.com>
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

#include "syconfig.h"
#include "syhowdy_server.h"

SyHowdyServer::SyHowdyServer(QObject *parent)
  : QObject(parent)
{
  d_server_socket=new SyMcastSocket(SyMcastSocket::ReadWrite,this);
  connect(d_server_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}


SyHowdyServer::~SyHowdyServer()
{
  delete d_server_socket;
}


bool SyHowdyServer::initialize(QString *err_msg)
{
  if(d_server_socket->bind(SWITCHYARD_WN_HOWDY_PORT)){
    *err_msg=tr("failed to bind port")+
      QString::asprintf(" %u",SWITCHYARD_WN_HOWDY_PORT);
    return false;
  }

  return true;
}


void SyHowdyServer::readyReadData()
{
  SyHowdyMessage *msg=new SyHowdyMessage(d_server_socket->receiveDatagram());
  emit messageReceived(msg);
  delete msg;
}
