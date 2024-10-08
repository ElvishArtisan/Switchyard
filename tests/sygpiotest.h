// sygpiotest.h
//
// Listen for GPIO events and print them to stdout
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYGPIOTEST_H
#define SYGPIOTEST_H

#include <QObject>

#include <sy/sygpio_server.h>
#include <sy/syrouting.h>

#define SYGPIOTEST_USAGE "--log [--iface-address=<ipv4-addr>\n"

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
  bool gpio_log;
};


#endif  // SYGPIOTEST_H
