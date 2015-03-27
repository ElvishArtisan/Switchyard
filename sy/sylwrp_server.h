// sylwrp_server.h
//
// Livewire Control Protocol Server
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef SYLWRP_SERVER_H
#define SYLWRP_SERVER_H

#include <vector>

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QSignalMapper>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>

#include <sy/syastring.h>
#include <sy/sylwrp_clientconnection.h>
#include <sy/syrouting.h>

class SyLwrpServer : public QObject
{
  Q_OBJECT
 public:
  SyLwrpServer(SyRouting *routing);

 private slots:
  void newConnectionData();
  void readData(int id);
  void closedData(int id);

 private:
  bool ExecuteLogin(int id,QStringList &args);
  bool ExecuteVer(int id,QStringList &args);
  bool ExecuteIp(int ch,QStringList &args);
  bool ExecuteSrc(int ch,QStringList &args);
  bool ExecuteDst(int ch,QStringList &args);
  bool ExecuteGpi(int ch,QStringList &args);
  bool ExecuteGpo(int ch,QStringList &args);
  bool ExecuteIfc(int ch,QStringList &args);
  QString SrcLine(int slot);
  QString DstLine(int slot);
  QString GpiLine(int slot);
  QString GpoLine(int slot);
  QStringList ParseField(const SyAString &field);
  void ParseCommand(int id);
  void BroadcastCommand(const QString &cmd);
  void SendCommand(int ch,const QString &cmd);
  void UnsubscribeStream(int slot);
  std::vector<SyLwrpClientConnection *>ctrl_client_connections;
  QTcpServer *ctrl_server;
  QSignalMapper *ctrl_read_mapper;
  QSignalMapper *ctrl_closed_mapper;
  SyRouting *ctrl_routing;
};


#endif  // SYLWRP_SERVER_H
