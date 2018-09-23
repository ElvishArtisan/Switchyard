// sylwrp_clientconnection.cpp
//
// Container class for LWRP client connections.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
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

#include "sylwrp_clientconnection.h"

SyLwrpClientConnection::SyLwrpClientConnection()
{
  client_socket=NULL;
  client_command_buffer="";
  client_gpi_added=false;
  client_gpo_added=false;
}


SyLwrpClientConnection::~SyLwrpClientConnection()
{
  client_socket->deleteLater();
}


QTcpSocket *SyLwrpClientConnection::socket() const
{
  return client_socket;
}


void SyLwrpClientConnection::setSocket(QTcpSocket *socket)
{
  client_socket=socket;
}


QString SyLwrpClientConnection::commandBuffer() const
{
  return client_command_buffer;
}


void SyLwrpClientConnection::appendCommandBuffer(const char c)
{
  client_command_buffer+=c;
}


void SyLwrpClientConnection::clearBuffer()
{
  client_command_buffer="";
}


bool SyLwrpClientConnection::gpiAdded() const
{
  return client_gpi_added;
}


void SyLwrpClientConnection::gpiAdd()
{
  client_gpi_added=true;
}


void SyLwrpClientConnection::gpiDel()
{
  client_gpi_added=false;
}


bool SyLwrpClientConnection::gpoAdded() const
{
  return client_gpo_added;
}


void SyLwrpClientConnection::gpoAdd()
{
  client_gpo_added=true;
}


void SyLwrpClientConnection::gpoDel()
{
  client_gpo_added=false;
}
