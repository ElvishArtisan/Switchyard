// rtp.h
//
// Real Time Protocol
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef RTP_H
#define RTP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "routing.h"

extern void RTPCallback(int sock,Routing *r,void *priv);

class RTPServer : public QObject
{
  Q_OBJECT;
 public:
  RTPServer(Routing *routing,void *priv,QObject *parent=0);

 signals:
  void exiting();

 private slots:
  void exitTimerData();

 private:
  pthread_t rtp_thread;
  QTimer *rtp_exit_timer;
};


#endif  // RTP_H
