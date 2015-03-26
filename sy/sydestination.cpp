// sydestination.cpp
//
// Container class for LiveWire destination attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include "sydestination.h"

SyDestination::SyDestination(const QHostAddress &s_addr,const QString &name)
{
  dst_stream_address=s_addr;
  dst_name=name;
}


SyDestination::SyDestination()
{
}


QHostAddress SyDestination::streamAddress() const
{
  return dst_stream_address;
}


void SyDestination::setStreamAddress(const QHostAddress &addr)
{
  dst_stream_address=addr;
}


QString SyDestination::name() const
{
  return dst_name;
}


void SyDestination::setName(const QString &name)
{
  dst_name=name;
}
