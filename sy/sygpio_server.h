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

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtNetwork/QHostAddress>
#include <QtCore/QTimer>

#include <sy/symcastsocket.h>
#include <sy/syrouting.h>

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
