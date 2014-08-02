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

#include <sy/syadv_server.h>
#include <sy/syconfig.h>
#include <sy/sygpio_server.h>
#include <sy/sylwrp_server.h>
#include <sy/syrouting.h>
#include <sy/syrtp_server.h>

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
  void gpoReceivedData(int gpi,int line,bool state,bool pulse);
  void exitData();

 private:
  SyRouting *syd_routing;
  SyGpioServer *syd_gpio;
  SyLwrpServer *syd_lwrp;
  SyAdvServer *syd_adv;
  SyRtpServer *syd_rtp;
};


#endif  // SYD_H
