// syd.h
//
// Switchyard stream swiching daemon
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYD_H
#define SYD_H

#include <stdint.h>

#include <QObject>

#include <sy/syadv_server.h>
#include <sy/syconfig.h>
#include <sy/sygpio_server.h>
#include <sy/sylwrp_server.h>
#include <sy/syrouting.h>
#include <sy/syrtp_server.h>

#define SYD_USAGE "-d\n"
#define SYD_PID_FILE "/var/run/syd.pid"
#define SYD_SLOTS 4

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void gpiReceivedData(int gpi,int line,bool state,bool pulse);
  void gpoReceivedData(int gpi,int line,bool state,bool pulse);
  void exitTimerData();
  void exitData();

 private:
  SyRouting *syd_routing;
  SyGpioServer *syd_gpio;
  SyLwrpServer *syd_lwrp;
  SyAdvServer *syd_adv;
  SyRtpServer *syd_rtp;
};


#endif  // SYD_H
