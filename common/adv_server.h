// adv_server.h
//
// Livewire Advertising Protocol Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef ADV_SERVER_H
#define ADV_SERVER_H

#include <vector>

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>
#include <QtCore/QTimer>

#include "adv_packet.h"
#include "adv_source.h"
#include "adv_tag.h"
#include "astring.h"

#include "routing.h"

class AdvServer : public QObject
{
  Q_OBJECT;
 public:
  enum AdvertType {Type0=0,Type1=1,Type2=2,TypeLast=3};
  AdvServer(Routing *r,bool read_only=false,QObject *parent=0);

 private slots:
  void readData();
  void expireData();
  void sendData();
  void saveSourcesData();

 private:
  void SendSourceUpdate(AdvertType type);
  void GenerateAdvertPacket(Packet *p,AdvertType type) const;
  int GetAdvertInterval() const;
  Source *GetSource(const QHostAddress &node_addr,unsigned slot);
  int TagIsSource(const Tag *tag) const;
  void ScheduleSourceSave();
  double GetTimestamp() const;
  QUdpSocket *ctrl_advert_socket;
  QTimer *ctrl_advert_timer;
  uint32_t ctrl_advert_seqno;
  double ctrl_advert_timestamp;
  AdvertType ctrl_advert_type;
  Routing *adv_routing;
  QTimer *ctrl_expire_timer;
  QTimer *ctrl_savesources_timer;
  std::vector<Source *>ctrl_sources;
};


#endif  // ADV_SERVER_H
