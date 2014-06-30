//   adv_astring.cpp
//
//   A String with quote mode
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: astring.cpp,v 1.1 2010/09/28 23:03:27 pcvs Exp $
//
//   All Rights Reserved.
//

#include <QtCore/QStringList>

#include "adv_astring.h"


AString::AString()
  : QString()
{
}


AString::AString(const AString &lhs)
  : QString(lhs)
{
}


AString::AString(const QString &lhs)
  : QString(lhs)
{
}


QStringList AString::split(const QString &sep,const QString &esc) const
{
  if(esc.isEmpty()) {
    return QString::split(sep);
  }
  QStringList list;
  bool escape=false;
  QChar e=esc.at(0);
  list.push_back(QString());
  for(int i=0;i<length();i++) {
    if(at(i)==e) {
      escape=!escape;
    }
    else {
      if((!escape)&&(mid(i,1)==sep)) {
	list.push_back(QString());
      }
      else {
	list.back()+=at(i);
      }
    }
  }
  return list;
}
