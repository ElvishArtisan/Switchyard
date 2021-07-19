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


bool SyLwrpClientConnection::gpiAdded(int slot) const
{
  return client_gpi_added.value(slot,false);
}


void SyLwrpClientConnection::gpiAdd(int slot)
{
  client_gpi_added[slot]=true;
}


void SyLwrpClientConnection::gpiDel(int slot)
{
  client_gpi_added[slot]=false;
}


bool SyLwrpClientConnection::gpoAdded(int slot) const
{
  return client_gpo_added.value(slot,false);
}


void SyLwrpClientConnection::gpoAdd(int slot)
{
  client_gpo_added[slot]=true;
}


void SyLwrpClientConnection::gpoDel(int slot)
{
  client_gpo_added[slot]=false;
}
