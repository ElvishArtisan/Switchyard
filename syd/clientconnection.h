// clientconnection.h
//
// Container class for TCP client connections.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: clientconnection.h,v 1.1 2010/09/28 23:03:27 pcvs Exp $
//
//   All Rights Reserved.
//

#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

class ClientConnection
{
 public:
  ClientConnection();
  ~ClientConnection();
  QTcpSocket *socket() const;
  void setSocket(QTcpSocket *socket);
  QString commandBuffer() const;
  void appendCommandBuffer(const char c);
  void clearBuffer();

 private:
  QTcpSocket *client_socket;
  QString client_command_buffer;
};


#endif  // CLIENTCONNECTION_H
