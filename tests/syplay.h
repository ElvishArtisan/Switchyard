// syplay.h
//
// Switchyard stream play command
//
// (C) 2014-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYPLAY_H
#define SYPLAY_H

#include <stdint.h>

#include <QObject>

#include <sndfile.h>

#include <sy5/syadv_server.h>
#include <sy5/syclock.h>
#include <sy5/syconfig.h>
#include <sy5/sylwrp_server.h>
#include <sy5/syrouting.h>
#include <sy5/syrtp_header.h>
#include <sy5/syrtp_server.h>

#define SYPLAY_USAGE "--input=<filename>\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void pllUpdatedData(double ratio,int offset);
  void sendRtpData();
  void exitTimerData();
  void exitData();

 private:
  SyRouting *cap_routing;
  SyLwrpServer *cap_lwrp;
  SyAdvServer *cap_adv;
  SyRtpServer *cap_rtp;
  SyRtpHeader *cap_rtp_hdr;
  SyClock *cap_clock;
  SNDFILE *cap_sf;
};


#endif  // SYPLAY_H
