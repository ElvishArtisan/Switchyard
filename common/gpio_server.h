// gpio_server.h
//
// Livewire GPIO Server
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef GPIO_SERVER_H
#define GPIO_SERVER_H

#include <stdint.h>

#include <vector>
#include <map>

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>
#include <QtCore/QTimer>

#include "routing.h"

class GpioServer : public QObject
{
  Q_OBJECT;
 public:
  GpioServer(Routing *r,QObject *parent=0);
  ~GpioServer();

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
  QUdpSocket *CreateSendSocket(uint16_t port);
  QUdpSocket *CreateRecvSocket(uint16_t port);
  QUdpSocket *gpio_gpi_send_socket;
  QUdpSocket *gpio_gpi_recv_socket;
  QUdpSocket *gpio_gpo_send_socket;
  QUdpSocket *gpio_gpo_recv_socket;
  Routing *gpio_routing;
  uint32_t gpio_serial;
  std::map<uint32_t,uint32_t> gpio_src_addr_serials;
};


#endif  // GPIO_SERVER_H
