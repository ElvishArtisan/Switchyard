// syd.h
//
// Switchyard stream swiching daemon
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <stdint.h>

#include <QtCore/QObject>

#include "config.h"
#include "routing.h"

#define SYD_USAGE "-d\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private:
  Routing *syd_routing;
};
