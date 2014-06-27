// lwtag.cpp
//
// Abstract a LiveWire Control Protocol tag.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lwtag.cpp,v 1.5 2010/10/21 10:44:28 pcvs Exp $
//
//   All Rights Reserved.
//

#include <lwtag.h>

LwTag::LwTag()
{
  lw_tag_type=LwTag::TagLast;
  lw_tag_length=-1;
}


QString LwTag::tagName() const
{
  return lw_tag_name;
}


void LwTag::setTagName(const QString &name)
{
  lw_tag_name=name;
}


LwTag::TagType LwTag::tagType() const
{
  return lw_tag_type;
}


QVariant LwTag::tagValue() const
{
  return lw_tag_value;
}


int LwTag::tagLength() const
{
  return lw_tag_length;
}


void LwTag::setTagValue(TagType type,const QVariant &value,int taglen)
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


void LwTag::setTagValue(TagType type,const QHostAddress &addr)
{
  lw_tag_type=type;
  lw_tag_value.setValue(addr.toIPv4Address());
}


QString LwTag::normalizeName(const QString &str)
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
