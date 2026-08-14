// sylo_message.h
//
// Abstract a WheatNet LO mMssage
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

#ifndef SYLO_MESSAGE_H
#define SYLO_MESSAGE_H

#include <QDateTime>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QString>

class SyLoMessage
{
 public:
  SyLoMessage(const QNetworkDatagram &dgram);
  SyLoMessage();
  int bladeId() const;
  void setBladeId(int bid);
  QString bladeName() const;
  void setBladeName(const QString &str);
  QDateTime timestamp() const;
  void setTimestamp(const QDateTime &dt);
  QString text() const;
  void setText(const QString &str);
  QHostAddress hostAddress() const;
  void setHostAddress(const QHostAddress &addr);
  QString dump() const;

 private:
  int d_blade_id;
  QString d_blade_name;
  QDateTime d_timestamp;
  QString d_text;
  QHostAddress d_host_address;
};


#endif  // SYLO_MESSAGE_H
