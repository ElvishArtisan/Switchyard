// tag.cpp
//
// Abstract a LiveWire Control Protocol tag.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include "adv_tag.h"

Tag::Tag()
{
  lw_tag_type=Tag::TagLast;
  lw_tag_length=-1;
}


QString Tag::tagName() const
{
  return lw_tag_name;
}


void Tag::setTagName(const QString &name)
{
  lw_tag_name=name;
}


Tag::TagType Tag::tagType() const
{
  return lw_tag_type;
}


QVariant Tag::tagValue() const
{
  return lw_tag_value;
}


int Tag::tagLength() const
{
  return lw_tag_length;
}


void Tag::setTagValue(TagType type,const QVariant &value,int taglen)
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


void Tag::setTagValue(TagType type,const QHostAddress &addr)
{
  lw_tag_type=type;
  lw_tag_value.setValue(addr.toIPv4Address());
}


QString Tag::normalizeName(const QString &str)
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
