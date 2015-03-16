// sytone.h
//
// Switchyard tone command
//
// (C) 2014-2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYTONE_H
#define SYTONE_H

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

#define SYTONE_USAGE "\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void sourceAddressChangedData(const QHostAddress &addr);
  void pllUpdatedData(double ratio,int offset);
  void sendRtpData();
  void exitTimerData();
  void exitData();

 private:
  SyRouting *tone_routing;
  SyLwrpServer *tone_lwrp;
  SyAdvServer *tone_adv;
  SyRtpServer *tone_rtp;
  SyRtpHeader *tone_rtp_hdr;
  SyClock *tone_clock;
  char tone_buffer[1452];
  int tone_max_offset;
  int tone_min_offset;
};


#endif  // SYTONE_H
