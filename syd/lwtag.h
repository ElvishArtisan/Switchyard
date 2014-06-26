// lwtag.h
//
// Abstract a LiveWire Control Protocol tag.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: lwtag.h,v 1.5 2010/10/21 10:44:28 pcvs Exp $
//
//   All Rights Reserved.
//

#ifndef LWTAG_H
#define LWTAG_H

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtNetwork/QHostAddress>

class LwTag
{
 public:
  enum TagType {TagType0=0,TagType1=1,TagString=3,TagMeter=4,
		TagType6=6,TagType7=7,TagType8=8,TagType9=9,
		TagLast=10};
  LwTag();
  QString tagName() const;
  void setTagName(const QString &name);
  TagType tagType() const;
  QVariant tagValue() const;
  int tagLength() const;
  void setTagValue(TagType type,const QVariant &value,int taglen=-1);
  void setTagValue(TagType type,const QHostAddress &addr);
  static QString normalizeName(const QString &str);

 private:
  QString lw_tag_name;
  QVariant lw_tag_value;
  TagType lw_tag_type;
  int lw_tag_length;
};


#endif  // LWTAG_H
