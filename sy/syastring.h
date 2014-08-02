//   syastring.h
//
//   A String with quote mode
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: astring.h,v 1.1 2010/09/28 23:03:27 pcvs Exp $
//
//   All Rights Reserved.

#ifndef SYASTRING_H
#define SYASTRING_H

#include <QtCore/QString>

class SyAString : public QString
{
 public:
  SyAString();
  SyAString(const SyAString &lhs);
  SyAString(const QString &lhs);  
  QStringList split(const QString &sep,const QString &esc="") const;
};


#endif  // SYASTRING_H
