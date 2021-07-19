// sycapture.h
//
// Switchyard stream capture command
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

#ifndef SYCAPTURE_H
#define SYCAPTURE_H

#include <stdint.h>

#include <QObject>

#include <sndfile.h>

#include <sy5/syconfig.h>
#include <sy5/sylwrp_server.h>
#include <sy5/syrouting.h>
#include <sy5/syrtp_server.h>

#define SYCAPTURE_USAGE "[--bits=16|24|32] --output=<filename>\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void exitTimerData();
  void exitData();

 private:
  SyRouting *cap_routing;
  SyLwrpServer *cap_lwrp;
  SyRtpServer *cap_rtp;
  SNDFILE *cap_sf;
};


#endif  // SYCAPTURE_H
