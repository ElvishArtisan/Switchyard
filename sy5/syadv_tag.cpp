// syadv_tag.cpp
//
// Abstract a LiveWire Control Protocol tag.
//
// (C) Copyright 2009-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "syadv_tag.h"

SyTag::SyTag()
{
  lw_tag_type=SyTag::TagLast;
  lw_tag_length=-1;
}


QString SyTag::tagName() const
{
  return lw_tag_name;
}


void SyTag::setTagName(const QString &name)
{
  lw_tag_name=name;
}


SyTag::TagType SyTag::tagType() const
{
  return lw_tag_type;
}


QVariant SyTag::tagValue() const
{
  return lw_tag_value;
}


int SyTag::tagLength() const
{
  return lw_tag_length;
}


void SyTag::setTagValue(TagType type,const QVariant &value,int taglen)
{
  lw_tag_type=type;
  if(taglen>0) {
    QByteArray data(value.toString().left(taglen).toUtf8());
    while(data.size()<taglen) {
      data.append((char)0);
    }
    lw_tag_value=data;
  }
  else {
    lw_tag_value=value;
  }
  lw_tag_length=taglen;
}


void SyTag::setTagValue(TagType type,const QHostAddress &addr)
{
  lw_tag_type=type;
  lw_tag_value.setValue(addr.toIPv4Address());
}


QString SyTag::normalizeName(const QString &str)
{
  if((0xff&str.toUtf8()[0])>176) {
    QString ret;
    ret="{";
    for(int i=0;i<str.length();i++) {
      ret+=QString().sprintf("%02X ",0xff&str.toUtf8()[i]);
    }
    ret=ret.left(ret.length()-1)+"}";
    return ret;
  }
  return str;
}
