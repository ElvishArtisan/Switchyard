//   astring.h
//
//   A String with quote mode
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: astring.h,v 1.1 2010/09/28 23:03:27 pcvs Exp $
//
//   All Rights Reserved.

#ifndef ASTRING_H
#define ASTRING_H

#include <QtCore/QString>

class AString : public QString
{
 public:
  AString();
  AString(const AString &lhs);
  AString(const QString &lhs);  
  QStringList split(const QString &sep,const QString &esc="") const;
};


#endif  // STRING_H
