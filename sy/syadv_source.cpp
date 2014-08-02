// syadv_source.cpp
//
// Container class for info about a Livewire source
//
// (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include "syadv_source.h"

SyAdvSource::SyAdvSource()
{
  src_slot=0;
  src_source_number=0;
  src_hardware_type=TypeUnknown;
  src_stream_type=StreamUnknown;
  src_is_changed=false;
}


QHostAddress SyAdvSource::nodeAddress() const
{
  return src_node_address;
}


void SyAdvSource::setNodeAddress(const QHostAddress &addr)
{
  if(addr!=src_node_address) {
    src_node_address=addr;
    if(nodeName().isEmpty()) {
      setNodeName(addr.toString());
    }
    src_is_changed=true;
  }
}


QString SyAdvSource::nodeName() const
{
  return src_node_name;
}


void SyAdvSource::setNodeName(const QString &str)
{
  if(str!=src_node_name) {
    src_node_name=str;
    src_is_changed=true;
  }
}


unsigned SyAdvSource::slot() const
{
  return src_slot;
}


void SyAdvSource::setSlot(unsigned slot)
{
  if(slot!=src_slot) {
    src_slot=slot;
    src_is_changed=true;
  }
}


unsigned SyAdvSource::sourceNumber() const
{
  return src_source_number;
}


void SyAdvSource::setSourceNumber(unsigned streamno)
{
  if(streamno!=src_source_number) {
    src_source_number=streamno;
    src_is_changed=true;
  }
}


QHostAddress SyAdvSource::streamAddress() const
{
  return src_stream_address;
}


void SyAdvSource::setStreamAddress(const QHostAddress &addr)
{
  if(addr!=src_stream_address) {
    src_stream_address=addr;
    src_is_changed=true;
  }
}


QString SyAdvSource::sourceName() const
{
  return src_source_name;
}


void SyAdvSource::setSourceName(const QString &str)
{
  if(str!=src_source_name) {
    src_source_name=str;
    src_is_changed=true;
  }
}


SyAdvSource::HardwareType SyAdvSource::hardwareType() const
{
  return src_hardware_type;
}


void SyAdvSource::setHardwareType(HardwareType type)
{
  if(type!=src_hardware_type) {
    src_hardware_type=type;
    src_is_changed=true;
  }
}


SyAdvSource::StreamType SyAdvSource::streamType() const
{
  return src_stream_type;
}


void SyAdvSource::setStreamType(StreamType type)
{
  if(type!=src_stream_type) {
    src_stream_type=type;
    src_is_changed=true;
  }
}


int SyAdvSource::lastTouched() const
{
  return lastTouched(QDateTime(QDate::currentDate(),QTime::currentTime()));
}


int SyAdvSource::lastTouched(const QDateTime &datetime) const
{
  return datetime.secsTo(src_datetime);
}


void SyAdvSource::touch()
{
  src_datetime=QDateTime(QDate::currentDate(),QTime::currentTime());
}


void SyAdvSource::touch(const QDateTime &datetime)
{
  src_datetime=datetime;
}


bool SyAdvSource::isChanged() const
{
  return src_is_changed;
}


void SyAdvSource::setSaved()
{
  src_is_changed=false;
}


QString SyAdvSource::hardwareString(HardwareType type)
{
  QString ret=QString().sprintf("Unknown (0x%04X)",type);
  switch(type) {
  case SyAdvSource::TypeAnalogNode:
    ret="Analog Node";
    break;

  case SyAdvSource::TypeMicNode:
    ret="Mic Node";
    break;

  case SyAdvSource::TypeMixEngine:
    ret="Mix Engine";
    break;

  case SyAdvSource::TypeElement:
    ret="Element";
    break;

  case SyAdvSource::TypeUnknown:
    break;
  }
  return ret;
}
