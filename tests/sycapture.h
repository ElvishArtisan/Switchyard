// sycapture.h
//
// Switchyard stream capture command
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYCAPTURE_H
#define SYCAPTURE_H

#include <stdint.h>

#include <QtCore/QObject>

#include <sndfile.h>

#include <sy/syconfig.h>
#include <sy/sylwrp_server.h>
#include <sy/syrouting.h>
#include <sy/syrtp_server.h>

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
