// lwrp_server.h
//
// Livewire Control Protocol Server
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef LWRP_SERVER_H
#define LWRP_SERVER_H

#include <vector>

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QSignalMapper>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>

#include "astring.h"

#include "lwrp_clientconnection.h"

#include "routing.h"

class LWRPServer : public QObject
{
  Q_OBJECT
 public:
  LWRPServer(Routing *routing);

 private slots:
  void newConnectionData();
  void readData(int id);
  void clockReadData();
  void clockStartData();
  void closedData(int id);

 private:
  bool ExecuteLogin(int id,QStringList &args);
  bool ExecuteVer(int id,QStringList &args);
  bool ExecuteIp(int ch,QStringList &args);
  bool ExecuteSrc(int ch,QStringList &args);
  bool ExecuteDst(int ch,QStringList &args);
  QString SrcLine(int slot);
  QString DstLine(int slot);
  QStringList ParseField(const AString &field);
  void ParseCommand(int id);
  void BroadcastCommand(const QString &cmd);
  void SendCommand(int ch,const QString &cmd);
  void UnsubscribeStream(int slot);
  std::vector<ClientConnection *>ctrl_client_connections;
  QTcpServer *ctrl_server;
  QSignalMapper *ctrl_read_mapper;
  QSignalMapper *ctrl_closed_mapper;
  QUdpSocket *ctrl_clock_socket;
  Routing *ctrl_routing;
};


#endif  // LWRP_SERVER_H
