// syadv_server.h
//
// Livewire Advertising Protocol Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef SYADV_SERVER_H
#define SYADV_SERVER_H

#include <vector>

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtNetwork/QHostAddress>
#include <QtCore/QTimer>

#include <sy/syadv_packet.h>
#include <sy/syadv_source.h>
#include <sy/syadv_tag.h>
#include <sy/syastring.h>
#include <sy/symcastsocket.h>
#include <sy/syrouting.h>

class SyAdvServer : public QObject
{
  Q_OBJECT;
 public:
  enum AdvertType {Type0=0,Type1=1,Type2=2,TypeLast=3};
  SyAdvServer(SyRouting *r,bool read_only=false,QObject *parent=0);
  ~SyAdvServer();

 private slots:
  void readData();
  void expireData();
  void sendAdvert0Data();
  void sendAdvert1Data();
  void sendAdvert2Data();
  void saveSourcesData();

 private:
  void SendSourceUpdate(AdvertType type);
  void GenerateAdvertPacket0(SyAdvPacket *p) const;
  void GenerateAdvertPacket1(SyAdvPacket *p) const;
  bool GenerateAdvertPacket2(SyAdvPacket *p,unsigned base_slot) const;
  int GetAdvertInterval() const;
  SyAdvSource *GetSource(const QHostAddress &node_addr,unsigned slot);
  int TagIsSource(const SyTag *tag) const;
  void ScheduleSourceSave();
  SyMcastSocket *ctrl_advert_socket;
  QTimer *ctrl_advert_timer0;
  QTimer *ctrl_advert_timer1;
  QTimer *ctrl_advert_timer2;
  uint32_t ctrl_advert_seqno;
  SyRouting *adv_routing;
  QTimer *ctrl_expire_timer;
  QTimer *ctrl_savesources_timer;
  std::vector<SyAdvSource *>ctrl_sources;
};


#endif  // SYADV_SERVER_H
