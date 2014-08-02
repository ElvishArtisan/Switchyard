// syadv_tag.cpp
//
// Abstract a LiveWire Control Protocol tag.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
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
    QByteArray data((const char *)value.toString().left(taglen).toAscii());
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
  if((0xff&str.toAscii()[0])>176) {
    QString ret;
    ret="{";
    for(int i=0;i<str.length();i++) {
      ret+=QString().sprintf("%02X ",0xff&str.toAscii()[i]);
    }
    ret=ret.left(ret.length()-1)+"}";
    return ret;
  }
  return str;
}
