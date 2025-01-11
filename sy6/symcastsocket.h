// symcastsocket.h
//
// A QUdpSocket for multicasting.
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYMCASTSOCKET_H
#define SYMCASTSOCKET_H

#include <stdint.h>

#include <QByteArray>
#include <QHostAddress>
#include <QObject>
#include <QUdpSocket>

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
  bool subscribe(const QHostAddress &addr);
  bool subscribe(const QString &addr);
  bool unsubscribe(const QHostAddress &addr);
  bool unsubscribe(const QString &addr);
  static QString socketErrorText(QAbstractSocket::SocketError err);

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
