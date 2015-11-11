// sylwrp_clientconnection.h
//
// Container class for LWRP client connections.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: clientconnection.h,v 1.1 2010/09/28 23:03:27 pcvs Exp $
//
//   All Rights Reserved.
//

#ifndef SYLWRP_CLIENTCONNECTION_H
#define SYLWRP_CLIENTCONNECTION_H

#include <QString>
#include <QTcpSocket>

class SyLwrpClientConnection
{
 public:
  SyLwrpClientConnection();
  ~SyLwrpClientConnection();
  QTcpSocket *socket() const;
  void setSocket(QTcpSocket *socket);
  QString commandBuffer() const;
  void appendCommandBuffer(const char c);
  void clearBuffer();

 private:
  QTcpSocket *client_socket;
  QString client_command_buffer;
};


#endif  // SYLWRP_CLIENTCONNECTION_H
