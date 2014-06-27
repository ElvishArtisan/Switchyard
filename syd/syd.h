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

#include "config.h"
#include "lwrp.h"
#include "routing.h"
#include "rtp.h"

#define SYD_USAGE "-d\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void exitData();

 private:
  Routing *syd_routing;
  LWRPServer *syd_lwrp;
  RTPServer *syd_rtp;
};


#endif  // SYD_H
