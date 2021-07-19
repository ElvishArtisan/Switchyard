// syadv_server.h
//
// Livewire Advertising Protocol Server
//
// (C) Copyright 2014-2021 Fred Gleason <fredg@paravelsystems.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of version 2.1 of the GNU Lesser General Public
//    License as published by the Free Software Foundation;
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, 
//    Boston, MA  02111-1307  USA
//

#ifndef SYADV_SERVER_H
#define SYADV_SERVER_H

#include <vector>

#include <QDateTime>
#include <QHostAddress>
#include <QString>
#include <QTimer>

#include <sy5/syadv_packet.h>
#include <sy5/syadv_source.h>
#include <sy5/syadv_tag.h>
#include <sy5/syastring.h>
#include <sy5/syethmonitor.h>
#include <sy5/symcastsocket.h>
#include <sy5/syrouting.h>

class SyAdvServer : public QObject
{
  Q_OBJECT;
 public:
  enum AdvertType {Type0=0,Type1=1,Type2=2,TypeLast=3};
  SyAdvServer(SyRouting *r,bool read_only=false,QObject *parent=0);
  SyAdvServer(SyRouting *r,SyEthMonitor *ethmon,bool read_only=false,
	      QObject *parent=0);
  ~SyAdvServer();

 private slots:
  void readData();
  void expireData();
  void sendAdvert0Data();
  void sendAdvert1Data();
  void sendAdvert2Data();
  void saveSourcesData();
  void interfaceStartedData();
  void interfaceStoppedData();

 private:
  void SendSourceUpdate(AdvertType type);
  void GenerateAdvertPacket0(SyAdvPacket *p) const;
  void GenerateAdvertPacket1(SyAdvPacket *p) const;
  bool GenerateAdvertPacket2(SyAdvPacket *p,unsigned base_slot) const;
  int GetAdvertInterval() const;
  SyAdvSource *GetSource(const QHostAddress &node_addr,unsigned slot);
  int TagIsSource(const SyTag *tag) const;
  void ScheduleSourceSave();
  void Initialize(bool read_only);
  SyMcastSocket *ctrl_advert_socket;
  QTimer *ctrl_advert_timer0;
  QTimer *ctrl_advert_timer1;
  QTimer *ctrl_advert_timer2;
  uint32_t ctrl_advert_seqno;
  SyRouting *adv_routing;
  SyEthMonitor *adv_eth_monitor;
  QTimer *ctrl_expire_timer;
  QTimer *ctrl_savesources_timer;
  std::vector<SyAdvSource *>ctrl_sources;
};


#endif  // SYADV_SERVER_H
