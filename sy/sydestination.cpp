// sydestination.cpp
//
// Container class for LiveWire destination attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <QStringList>

#include "sydestination.h"

SyDestination::SyDestination()
  : SyEndPoint()
{
  dst_channels=2;
}


QHostAddress SyDestination::streamAddress() const
{
  return dst_stream_address;
}


void SyDestination::setStreamAddress(const QString &addr)
{
  QStringList f0=addr.split(" ");
  setStreamAddress(QHostAddress(f0[0]));
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


unsigned SyDestination::channels() const
{
  return dst_channels;
}


void SyDestination::setChannels(unsigned chans)
{
  dst_channels=chans;
}
