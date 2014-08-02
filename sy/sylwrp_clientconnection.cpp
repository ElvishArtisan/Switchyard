// sylwrp_clientconnection.cpp
//
// Container class for LWRP client connections.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
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
