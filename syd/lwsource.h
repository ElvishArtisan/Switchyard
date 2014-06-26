// lwsource.h
//
// Container class for info about a Livewire source
//
// (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lwsource.h,v 1.1 2010/10/02 22:14:38 pcvs Exp $
//
//   All Rights Reserved.
//

#ifndef LWSOURCE_H
#define LWSOURCE_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtNetwork/QHostAddress>

class LwSource
{
 public:
  enum HardwareType {TypeUnknown=0,TypeAnalogNode=0x1404,TypeMicNode=0x1403,
		     TypeMixEngine=0x1406,TypeElement=0x1407};
  enum StreamType {StreamUnknown=0,StreamFast=1,StreamMedium=2,StreamSlow=3,
		   StreamSurround=4};
  LwSource();
  QHostAddress nodeAddress() const;
  void setNodeAddress(const QHostAddress &addr);
  unsigned slot() const;
  void setSlot(unsigned slot);
  unsigned sourceNumber() const;
  void setSourceNumber(unsigned streamno);
  QHostAddress streamAddress() const;
  void setStreamAddress(const QHostAddress &addr);
  HardwareType hardwareType() const;
  void setHardwareType(HardwareType type);
  StreamType streamType() const;
  void setStreamType(StreamType type);
  int lastTouched() const;
  int lastTouched(const QDateTime &datetime) const;
  void touch();
  void touch(const QDateTime &datetime);
  static QString hardwareString(HardwareType type);

 private:
  QHostAddress src_node_address;
  unsigned src_slot;
  unsigned src_source_number;
  QHostAddress src_stream_address;
  HardwareType src_hardware_type;
  StreamType src_stream_type;
  QDateTime src_datetime;
};


#endif  // LWTAG_H
