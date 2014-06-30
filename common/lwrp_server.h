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

#include "adv_astring.h"
#include "adv_packet.h"
#include "adv_source.h"
#include "adv_tag.h"

#include "lwrp_clientconnection.h"

#include "routing.h"

class LWRPServer : public QObject
{
  Q_OBJECT
 public:
  enum AdvertType {Type0=0,Type1=1,Type2=2,TypeLast=3};
  LWRPServer(Routing *routing);

 private slots:
  void newConnectionData();
  void readData(int id);
  void advertReadData();
  void advertExpireData();
  void advertSendData();
  void clockReadData();
  void clockStartData();
  void saveSourcesData();
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
  Source *GetSource(const QHostAddress &node_addr,unsigned slot);
  int TagIsSource(const Tag *tag) const;
  void SendSourceUpdate(AdvertType type);
  void GenerateAdvertPacket(Packet *p,AdvertType type) const;
  int GetAdvertInterval() const;
  double GetTimestamp() const;
  void ScheduleSourceSave();
  std::vector<ClientConnection *>ctrl_client_connections;
  std::vector<Source *>ctrl_sources;
  QTcpServer *ctrl_server;
  QSignalMapper *ctrl_read_mapper;
  QSignalMapper *ctrl_closed_mapper;
  QUdpSocket *ctrl_advert_socket;
  QUdpSocket *ctrl_clock_socket;
  Routing *ctrl_routing;
  QTimer *ctrl_expire_timer;
  QTimer *ctrl_advert_timer;
  QTimer *ctrl_savesources_timer;
  uint32_t ctrl_advert_seqno;
  double ctrl_advert_timestamp;
  AdvertType ctrl_advert_type;
};


#endif  // LWRP_SERVER_H
