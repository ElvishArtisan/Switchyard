// lwsource.cpp
//
// Container class for info about a Livewire source
//
// (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lwsource.cpp,v 1.1 2010/10/02 22:14:38 pcvs Exp $
//
//   All Rights Reserved.
//

#include <lwsource.h>

LwSource::LwSource()
{
  src_slot=0;
  src_source_number=0;
  src_hardware_type=TypeUnknown;
  src_stream_type=StreamUnknown;
}


QHostAddress LwSource::nodeAddress() const
{
  return src_node_address;
}


void LwSource::setNodeAddress(const QHostAddress &addr)
{
  src_node_address=addr;
}


unsigned LwSource::slot() const
{
  return src_slot;
}


void LwSource::setSlot(unsigned slot)
{
  src_slot=slot;
}


unsigned LwSource::sourceNumber() const
{
  return src_source_number;
}


void LwSource::setSourceNumber(unsigned streamno)
{
  src_source_number=streamno;
}


QHostAddress LwSource::streamAddress() const
{
  return src_stream_address;
}


void LwSource::setStreamAddress(const QHostAddress &addr)
{
  src_stream_address=addr;
}


LwSource::HardwareType LwSource::hardwareType() const
{
  return src_hardware_type;
}


void LwSource::setHardwareType(HardwareType type)
{
  src_hardware_type=type;
}


LwSource::StreamType LwSource::streamType() const
{
  return src_stream_type;
}


void LwSource::setStreamType(StreamType type)
{
  src_stream_type=type;
}


int LwSource::lastTouched() const
{
  return lastTouched(QDateTime(QDate::currentDate(),QTime::currentTime()));
}


int LwSource::lastTouched(const QDateTime &datetime) const
{
  return datetime.secsTo(src_datetime);
}


void LwSource::touch()
{
  src_datetime=QDateTime(QDate::currentDate(),QTime::currentTime());
}


void LwSource::touch(const QDateTime &datetime)
{
  src_datetime=datetime;
}


QString LwSource::hardwareString(HardwareType type)
{
  QString ret=QString().sprintf("Unknown (0x%04X)",type);
  switch(type) {
  case LwSource::TypeAnalogNode:
    ret="Analog Node";
    break;

  case LwSource::TypeMicNode:
    ret="Mic Node";
    break;

  case LwSource::TypeMixEngine:
    ret="Mix Engine";
    break;

  case LwSource::TypeElement:
    ret="Element";
    break;

  case LwSource::TypeUnknown:
    break;
  }
  return ret;
}
