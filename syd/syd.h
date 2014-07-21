// syd.h
//
// Switchyard stream swiching daemon
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYD_H
#define SYD_H

#include <stdint.h>

#include <QtCore/QObject>

#include "adv_server.h"
#include "config.h"
#include "gpio_server.h"
#include "lwrp_server.h"
#include "routing.h"
#include "rtp.h"

#define SYD_USAGE "-d\n"
#define SYD_PID_FILE "/var/run/syd.pid"
#define SYD_SLOTS 1

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void gpiReceivedData(int gpi,int line,bool state,bool pulse);
  void exitData();

 private:
  Routing *syd_routing;
  GpioServer *syd_gpio;
  LWRPServer *syd_lwrp;
  AdvServer *syd_adv;
  RTPServer *syd_rtp;
};


#endif  // SYD_H
