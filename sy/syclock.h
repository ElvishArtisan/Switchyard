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

#include <queue>

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <sy/symcastsocket.h>

#define SYCLOCK_WINDOW_SIZE 100.0

class SyClock : public QObject
{
  Q_OBJECT;
 public:
  SyClock(QObject *parent=0);
  ~SyClock();
  double pllRatio() const;

 signals:
  void sendRtp();

 private slots:
  void readyReadData();
  void pllData();
  void sendRtpData();

 private:
  double GetTime();
  SyMcastSocket *clock_socket;
  QTimer *clock_pll_timer;
  double clock_pll_ratio;
  QTimer *clock_rtp_timer;
  uint64_t clock_counter;
  int clock_burst_counter;
  std::queue<double> clock_tick_window;
  bool clock_window_active;
};


#endif  // SYCLOCK_H
