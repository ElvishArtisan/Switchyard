//   syastring.h
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

#ifndef SYASTRING_H
#define SYASTRING_H

#include <QString>

class SyAString : public QString
{
 public:
  SyAString();
  SyAString(const SyAString &lhs);
  SyAString(const QString &lhs);  
  QStringList split(const QString &sep,const QString &esc="") const;
};


#endif  // SYASTRING_H
