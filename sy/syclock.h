// syclock.h
//
// Livewire Clock
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef SYCLOCK_H
#define SYCLOCK_H

#include <stdint.h>
#include <sys/time.h>
#include <time.h>

#include <queue>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>

#include <sy/symcastsocket.h>

#define SYCLOCK_WINDOW_SIZE 100.0
#define SYCLOCK_WATCHDOG_INTERVAL 5000

class SyClock : public QObject
{
  Q_OBJECT;
 public:
  SyClock(QObject *parent=0);
  ~SyClock();
  QHostAddress sourceAddress() const;

 signals:
  void sendRtp();
  void pllHasReset();
  void pllUpdated(int interval,int offset);
  void sourceAddressChanged(const QHostAddress &addr);
  void watchdogChanged(bool state);

 private slots:
  void readyReadData();
  void pllData();
  void sendRtpData();
  void watchdogData();

 private:
  QHostAddress clock_source_address;
  SyMcastSocket *clock_socket;
  QTimer *clock_pll_timer;
  QTimer *clock_rtp_timer;
  uint64_t clock_counter;
  int clock_burst_counter;
  uint32_t clock_clock_frame;
  uint32_t clock_pcm_frame;
  uint32_t clock_diff_clock_frame;
  uint32_t clock_diff_pcm_frame;
  int64_t clock_diff_setpoint;
  uint32_t clock_clock_count;
  int clock_pll_interval;
  QTimer *clock_watchdog_timer;
  bool clock_watchdog_state;
};


#endif  // SYCLOCK_H
