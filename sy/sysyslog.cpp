// sysyslog.cpp
//
// Cross-platform system logging routines for Switchyard
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdlib.h>

#include "sysyslog.h"

void SyOpenLog(const QString &ident,int option,int facility)
{
#ifndef WIN32
  openlog(ident.toAscii(),option,facility);
#endif  // WIN32
}


void SySyslog(int priority,const QString &msg)
{
#ifdef WIN32
  fprintf(stderr,"%s\n",(const char *)msg.toAscii());
#else
  syslog(priority,"%s",(const char *)msg.toAscii());
#endif  // WIN32
}
