// lwrp_clientconnection.cpp
//
// Container class for LWRP client connections.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include "lwrp_clientconnection.h"

ClientConnection::ClientConnection()
{
  client_socket=NULL;
  client_command_buffer="";
}


ClientConnection::~ClientConnection()
{
  client_socket->deleteLater();
}


QTcpSocket *ClientConnection::socket() const
{
  return client_socket;
}


void ClientConnection::setSocket(QTcpSocket *socket)
{
  client_socket=socket;
}


QString ClientConnection::commandBuffer() const
{
  return client_command_buffer;
}


void ClientConnection::appendCommandBuffer(const char c)
{
  client_command_buffer+=c;
}


void ClientConnection::clearBuffer()
{
  client_command_buffer="";
}
