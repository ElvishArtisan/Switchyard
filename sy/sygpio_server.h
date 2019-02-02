// sygpio_server.h
//
// Livewire GPIO Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYGPIO_SERVER_H
#define SYGPIO_SERVER_H

#include <stdint.h>

#include <vector>
#include <map>

#include <QDateTime>
#include <QHostAddress>
#include <QMap>
#include <QString>
#include <QTimer>

#include <sy/symcastsocket.h>
#include <sy/syethmonitor.h>
#include <sy/syrouting.h>

class SyGpioBundleEvent
{
 public:
  enum Type {TypeGpi=0,TypeGpo=1};
  SyGpioBundleEvent(Type type,const QHostAddress &orig_addr,uint16_t orig_port,
		    int srcnum,const QString &code);
  Type type() const;
  QHostAddress originAddress() const;
  uint16_t originPort() const;
  QHostAddress sourceAddress() const;
  int sourceNumber() const;
  QString code() const;
  QString dump() const;

 private:
  Type event_type;
  QHostAddress event_origin_address;
  uint16_t event_origin_port;
  int event_source_number;
  QString event_code;
};




class SyGpioEvent
{
 public:
  enum Type {TypeGpi=0,TypeGpo=1};
  SyGpioEvent(Type type,const QHostAddress &orig_addr,uint16_t orig_port,
	      int srcnum,int line,bool state,bool pulse);
  Type type() const;
  QHostAddress originAddress() const;
  uint16_t originPort() const;
  QHostAddress sourceAddress() const;
  int sourceNumber() const;
  int line() const;
  bool state() const;
  bool isPulse() const;
  QString dump() const;

 private:
  Type event_type;
  QHostAddress event_origin_address;
  uint16_t event_origin_port;
  int event_source_number;
  int event_line;
  bool event_state;
  bool event_pulse;
};




class SyGpioServer : public QObject
{
  Q_OBJECT;
 public:
  SyGpioServer(SyRouting *r,QObject *parent=0);
  SyGpioServer(SyRouting *r,SyEthMonitor *ethmon,QObject *parent=0);
  ~SyGpioServer();

 public slots:
  void sendGpi(int srcnum,int line,bool state,bool pulse);
  void sendGpi(int srcnum,const QString &code);
  void sendGpo(int srcnum,int line,bool state,bool pulse);
  void sendGpo(int srcnum,const QString &code,bool pulse);

 signals:
  void gpioReceived(SyGpioBundleEvent *e);
  void gpioReceived(SyGpioEvent *e);
  void gpiReceived(int gpi,int line,bool state,bool pulse);
  void gpoReceived(int gpo,int line,bool state,bool pulse);

 private slots:
  void interfaceStartedData();
  void interfaceStoppedData();
  void gpiReadyReadData();
  void gpoReadyReadData();

 private:
  SyMcastSocket *gpio_gpi_socket;
  SyMcastSocket *gpio_gpo_socket;
  SyRouting *gpio_routing;
  uint32_t gpio_serial;
  std::map<uint32_t,uint32_t> gpio_src_addr_serials;
  SyEthMonitor *gpio_eth_monitor;
  QMap<int,QString> gpio_gpi_codes;
};


#endif  // SYGPIO_SERVER_H
