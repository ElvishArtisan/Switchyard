// sydestination.cpp
//
// Container class for LiveWire destination attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
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


QString SyDestination::dump() const
{
  QString ret="";

  ret+="streamAddress: "+streamAddress().toString()+"\n";
  ret+="name: "+name()+"\n";
  ret+=QString().sprintf("channels: %u\n",channels());

  return ret;
}
