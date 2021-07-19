// syadv_tag.h
//
// Abstract a LiveWire Control Protocol tag.
//
// (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYADV_TAG_H
#define SYADV_TAG_H

#include <QHostAddress>
#include <QString>
#include <QVariant>

class SyTag
{
 public:
  enum TagType {TagType0=0,TagType1=1,TagString=3,TagMeter=4,
		TagType6=6,TagType7=7,TagType8=8,TagType9=9,
		TagLast=10};
  SyTag();
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


#endif  // SYADV_TAG_H
