// symessage.h
//
// Abstract base class for messages.
//
// (C) Copyright 2026 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYMESSAGE_H
#define SYMESSAGE_H

#include <QByteArray>
#include <QHostAddress>
#include <QNetworkDatagram>

class SyMessage
{
 public:
  SyMessage(const QNetworkDatagram &dgram);
  SyMessage();
  QByteArray body() const;
  QHostAddress hostAddress() const;
  void setHostAddress(const QHostAddress &addr);
  virtual QString dump()=0;
  virtual QString dumpBody();
  QString toAscii();
  static QString toAscii(const QByteArray &data);

 private:
  QByteArray d_body;
  QHostAddress d_host_address;
};


#endif  // SYMESSAGE_H
