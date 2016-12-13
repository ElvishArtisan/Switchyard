// sygpio_server.h
//
// Livewire GPIO Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef SYGPIO_SERVER_H
#define SYGPIO_SERVER_H

#include <stdint.h>

#include <vector>
#include <map>

#include <QDateTime>
#include <QHostAddress>
#include <QString>
#include <QTimer>

#include <sy/symcastsocket.h>
#include <sy/syrouting.h>

class SyGpioEvent
{
 public:
  enum Type {TypeGpi=0,TypeGpo=1};
  SyGpioEvent(Type type,const QHostAddress &orig_addr,uint16_t orig_port,
	      int srcnum,int line,bool state,bool pulse);
  Type type() const;
  QHostAddress originAddress() const;
  uint16_t originPort() const;
  int sourceNumber() const;
  int line() const;
  bool state() const;
  bool isPulse() const;

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
  ~SyGpioServer();

 public slots:
  void sendGpi(int gpi,int line,bool state,bool pulse);
  void sendGpo(int gpo,int line,bool state,bool pulse);

 signals:
  void gpioReceived(SyGpioEvent *e);
  void gpiReceived(int gpi,int line,bool state,bool pulse);
  void gpoReceived(int gpo,int line,bool state,bool pulse);

 private slots:
  void gpiReadyReadData();
  void gpoReadyReadData();

 private:
  SyMcastSocket *gpio_gpi_socket;
  SyMcastSocket *gpio_gpo_socket;
  SyRouting *gpio_routing;
  uint32_t gpio_serial;
  std::map<uint32_t,uint32_t> gpio_src_addr_serials;
};


#endif  // SYGPIO_SERVER_H
