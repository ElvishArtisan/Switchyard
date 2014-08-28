// sysyslog.cpp
//
// Cross-platform system logging routines for Switchyard
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved
//

#ifndef WIN32
#include <syslog.h>
#endif  // WIN32

#include "sysyslog.h"

void SyOpenLog(const QString &ident,int option,int facility)
{
#ifndef WIN32
  openlog(ident.toAscii(),option,facility);
#endif  // WIN32
}


void SySyslog(int priority,const QString &msg)
{
#ifndef WIN32
  syslog(priority,"%s",(const char *)msg.toAscii());
#endif  // WIN32
}
