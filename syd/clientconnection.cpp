// clientconnection.cpp
//
// Container class for TCP client connections.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: clientconnection.cpp,v 1.1 2010/09/28 23:03:27 pcvs Exp $
//
//   All Rights Reserved.
//

#include <clientconnection.h>

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
