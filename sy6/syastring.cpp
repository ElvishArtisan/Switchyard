//   syastring.cpp
//
//   A String with quote mode
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
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

#include <QStringList>

#include "syastring.h"


SyAString::SyAString()
  : QString()
{
}


SyAString::SyAString(const SyAString &lhs)
  : QString(lhs)
{
}


SyAString::SyAString(const QString &lhs)
  : QString(lhs)
{
}


QStringList SyAString::split(const QString &sep,const QString &esc) const
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
