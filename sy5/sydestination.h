// sydestination.h
//
// Container class for LiveWire destination attributes
//
// (C) 2015-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYDESTINATION_H
#define SYDESTINATION_H

#include <QHostAddress>
#include <QString>

#include <sy5/syendpoint.h>

class SyDestination : public SyEndPoint
{
 public:
  SyDestination();
  QHostAddress streamAddress() const;
  void setStreamAddress(const QString &addr);
  void setStreamAddress(const QHostAddress &addr);
  QString name() const;
  void setName(const QString &name);
  unsigned channels() const;
  void setChannels(unsigned chans);
  QString dump() const;

 private:
  QHostAddress dst_stream_address;
  QString dst_name;
  unsigned dst_channels;
};


#endif  // SYDESTINATION_H
