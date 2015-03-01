// syplay.h
//
// Switchyard stream play command
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYPLAY_H
#define SYPLAY_H

#include <stdint.h>

#include <QtCore/QObject>

#include <sndfile.h>

#include <sy/syadv_server.h>
#include <sy/syclock.h>
#include <sy/syconfig.h>
#include <sy/sylwrp_server.h>
#include <sy/syrouting.h>
#include <sy/syrtp_header.h>
#include <sy/syrtp_server.h>

#define SYPLAY_USAGE "--input=<filename>\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
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
