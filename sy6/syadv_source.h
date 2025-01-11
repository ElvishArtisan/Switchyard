// syadv_source.h
//
// Container class for info about a Livewire source
//
// (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYADV_SOURCE_H
#define SYADV_SOURCE_H

#include <QDateTime>
#include <QHostAddress>
#include <QString>

class SyAdvSource
{
 public:
  enum HardwareType {TypeUnknown=0,TypeAnalogNode=0x1404,TypeMicNode=0x1403,
		     TypeMixEngine=0x1406,TypeElement=0x1407};
  enum StreamType {StreamUnknown=0,StreamFast=1,StreamMedium=2,StreamSlow=3,
		   StreamSurround=4};
  SyAdvSource();
  QHostAddress nodeAddress() const;
  void setNodeAddress(const QHostAddress &addr);
  QString nodeName() const;
  void setNodeName(const QString &str);
  unsigned slot() const;
  void setSlot(unsigned slot);
  unsigned sourceNumber() const;
  void setSourceNumber(unsigned streamno);
  QHostAddress streamAddress() const;
  void setStreamAddress(const QHostAddress &addr);
  QString sourceName() const;
  void setSourceName(const QString &str);
  HardwareType hardwareType() const;
  void setHardwareType(HardwareType type);
  StreamType streamType() const;
  void setStreamType(StreamType type);
  int lastTouched() const;
  int lastTouched(const QDateTime &datetime) const;
  void touch();
  void touch(const QDateTime &datetime);
  bool isChanged() const;
  void setSaved();
  static QString hardwareString(HardwareType type);

 private:
  QHostAddress src_node_address;
  QString src_node_name;
  QString src_source_name;
  unsigned src_slot;
  unsigned src_source_number;
  QHostAddress src_stream_address;
  HardwareType src_hardware_type;
  StreamType src_stream_type;
  QDateTime src_datetime;
  bool src_is_changed;
};


#endif  // SYADV_SOURCE_H
