// sysource.h
//
// Container class for LiveWire source attributes
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

#ifndef SYSOURCE_H
#define SYSOURCE_H

#include <QHostAddress>
#include <QString>

#include <sy5/syendpoint.h>

class SySource : public SyEndPoint
{
 public:
  SySource(const QHostAddress &s_addr,const QString &name,bool enabled);
  SySource();
  QHostAddress streamAddress() const;
  void setStreamAddress(const QHostAddress &addr);
  QString name() const;
  void setName(const QString &name);
  QString label() const;
  void setLabel(const QString &str);
  bool enabled() const;
  void setEnabled(bool state);
  unsigned channels() const;
  void setChannels(unsigned chans);
  unsigned packetSize() const;
  void setPacketSize(unsigned size);
  bool shareable() const;
  void setShareable(bool state);
  QString dump() const;

 private:
  QHostAddress src_stream_address;
  QString src_name;
  QString src_label;
  bool src_enabled;
  unsigned src_channels;
  unsigned src_packet_size;
  bool src_shareable;
};


#endif  // SYSOURCE_H
