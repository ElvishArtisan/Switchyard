// symcastsocket.h
//
// A QUdpSocket for multicasting.
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef SYMCASTSOCKET_H
#define SYMCASTSOCKET_H

#include <stdint.h>

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>

class SyMcastSocket : public QObject
{
  Q_OBJECT;
 public:
  enum Mode {ReadOnly=0,WriteOnly=1,ReadWrite=2};
  SyMcastSocket(Mode mode,QObject *parent=0);
  ~SyMcastSocket();
  Mode mode() const;
  bool bind(const QHostAddress &addr,uint16_t port);
  bool bind(uint16_t port);
  qint64 readDatagram(char *data,qint64 len,
		       QHostAddress *addr=NULL,quint16 *port=NULL);
  qint64 writeDatagram(const char *data,qint64 len,
			const QHostAddress &addr,quint16 port);
  qint64 writeDatagram(const QByteArray &datagram,
			const QHostAddress &addr,quint16 port);
  void subscribe(const QHostAddress &addr);
  void subscribe(const QString &addr);
  void unsubscribe(const QHostAddress &addr);
  void unsubscribe(const QString &addr);

 signals:
  void readyRead();

 private slots:
  void readyReadData();

 private:
  QUdpSocket *mcast_send_socket;
  QUdpSocket *mcast_recv_socket;
  Mode mcast_mode;
  QHostAddress mcast_iface_address;
};


#endif  // SYMCASTSOCKET_H
