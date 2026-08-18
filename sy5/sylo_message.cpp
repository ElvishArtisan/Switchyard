// sylo_message.cpp
//
// Abstract a WheatNet LO mMssage
//
// (C) Copyright 2026 Fred Gleason <fredg@paravelsystems.com>
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

#include <QObject>

#include "sylo_message.h"

SyLoMessage::SyLoMessage(const QNetworkDatagram &dgram)
  : SyMessage(dgram)
{
  QDateTime now=QDateTime::currentDateTime();
  
  d_blade_id=QString(dgram.data().mid(8,3)).toInt();
  d_blade_name=QString::fromUtf8(dgram.data().mid(13,8)).trimmed();
  d_text=QString::fromUtf8(dgram.data().right(dgram.data().size()-49));
  d_timestamp=now;
  //  d_timestamp=QDateTime::fromString(dgram.data().mid(23,5)+now.toString("yyyy")+,"MM/dd/yyyy hh:mm:ss");
}


SyLoMessage::SyLoMessage()
  : SyMessage()
{
  /*
  d_timestamp=QDateTime::currentDateTime();
  d_blade_id=0;
  d_blade_name="??";
  */
}


int SyLoMessage::bladeId() const
{
  return d_blade_id;
}


void SyLoMessage::setBladeId(int bid)
{
  d_blade_id=bid;
}


QString SyLoMessage::bladeName() const
{
  return d_blade_name;
}


void SyLoMessage::setBladeName(const QString &str)
{
  d_blade_name=str;
}


QDateTime SyLoMessage::timestamp() const
{
  return d_timestamp;
}


void SyLoMessage::setTimestamp(const QDateTime &dt)
{
  d_timestamp=dt;
}


QString SyLoMessage::text() const
{
  return d_text;
}


void SyLoMessage::setText(const QString &str)
{
  d_text=str;
}


QString SyLoMessage::dump()
{
  QString ret="";

  ret+=d_timestamp.toString("MMM dd hh:mm:ss")+" ";
  //  ret+=QObject::tr("From blade")+QString::asprintf( " %d ",d_blade_id);
  ret+=d_blade_name+"["+hostAddress().toString()+"]: ";
  ret+=d_text;
  
  return ret;
}
