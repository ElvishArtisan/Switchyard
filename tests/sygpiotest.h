// sygpiotest.h
//
// Listen for GPIO events and print them to stdout
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYGPIOTEST_H
#define SYGPIOTEST_H

#include <QObject>

#include <sy/sygpio_server.h>
#include <sy/syrouting.h>

#define SYGPIOTEST_USAGE "\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void gpioReceivedData(SyGpioEvent *e);

 private:
  SyRouting *gpio_routing;
  SyGpioServer *gpio_server;
};


#endif  // SYGPIOTEST_H
