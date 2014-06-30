// source.cpp
//
// Container class for info about a Livewire source
//
// (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include "adv_source.h"

Source::Source()
{
  src_slot=0;
  src_source_number=0;
  src_hardware_type=TypeUnknown;
  src_stream_type=StreamUnknown;
  src_is_changed=false;
}


QHostAddress Source::nodeAddress() const
{
  return src_node_address;
}


void Source::setNodeAddress(const QHostAddress &addr)
{
  if(addr!=src_node_address) {
    src_node_address=addr;
    if(nodeName().isEmpty()) {
      setNodeName(addr.toString());
    }
    src_is_changed=true;
  }
}


QString Source::nodeName() const
{
  return src_node_name;
}


void Source::setNodeName(const QString &str)
{
  if(str!=src_node_name) {
    src_node_name=str;
    src_is_changed=true;
  }
}


unsigned Source::slot() const
{
  return src_slot;
}


void Source::setSlot(unsigned slot)
{
  if(slot!=src_slot) {
    src_slot=slot;
    src_is_changed=true;
  }
}


unsigned Source::sourceNumber() const
{
  return src_source_number;
}


void Source::setSourceNumber(unsigned streamno)
{
  if(streamno!=src_source_number) {
    src_source_number=streamno;
    src_is_changed=true;
  }
}


QHostAddress Source::streamAddress() const
{
  return src_stream_address;
}


void Source::setStreamAddress(const QHostAddress &addr)
{
  if(addr!=src_stream_address) {
    src_stream_address=addr;
    src_is_changed=true;
  }
}


QString Source::sourceName() const
{
  return src_source_name;
}


void Source::setSourceName(const QString &str)
{
  if(str!=src_source_name) {
    src_source_name=str;
    src_is_changed=true;
  }
}


Source::HardwareType Source::hardwareType() const
{
  return src_hardware_type;
}


void Source::setHardwareType(HardwareType type)
{
  if(type!=src_hardware_type) {
    src_hardware_type=type;
    src_is_changed=true;
  }
}


Source::StreamType Source::streamType() const
{
  return src_stream_type;
}


void Source::setStreamType(StreamType type)
{
  if(type!=src_stream_type) {
    src_stream_type=type;
    src_is_changed=true;
  }
}


int Source::lastTouched() const
{
  return lastTouched(QDateTime(QDate::currentDate(),QTime::currentTime()));
}


int Source::lastTouched(const QDateTime &datetime) const
{
  return datetime.secsTo(src_datetime);
}


void Source::touch()
{
  src_datetime=QDateTime(QDate::currentDate(),QTime::currentTime());
}


void Source::touch(const QDateTime &datetime)
{
  src_datetime=datetime;
}


bool Source::isChanged() const
{
  return src_is_changed;
}


void Source::setSaved()
{
  src_is_changed=false;
}


QString Source::hardwareString(HardwareType type)
{
  QString ret=QString().sprintf("Unknown (0x%04X)",type);
  switch(type) {
  case Source::TypeAnalogNode:
    ret="Analog Node";
    break;

  case Source::TypeMicNode:
    ret="Mic Node";
    break;

  case Source::TypeMixEngine:
    ret="Mix Engine";
    break;

  case Source::TypeElement:
    ret="Element";
    break;

  case Source::TypeUnknown:
    break;
  }
  return ret;
}
