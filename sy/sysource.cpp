// sysource.cpp
//
// Container class for LiveWire source attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include "sysource.h"

SySource::SySource(const QHostAddress &s_addr,const QString &name,bool enabled)
{
  src_stream_address=s_addr;
  src_name=name;
  src_enabled=enabled;
}


SySource::SySource()
{
}


QHostAddress SySource::streamAddress() const
{
  return src_stream_address;
}


void SySource::setStreamAddress(const QHostAddress &addr)
{
  src_stream_address=addr;
}


QString SySource::name() const
{
  return src_name;
}


void SySource::setName(const QString &name)
{
  src_name=name;
}


bool SySource::enabled() const
{
  return src_enabled;
}


void SySource::setEnabled(bool state)
{
  src_enabled=state;
}
