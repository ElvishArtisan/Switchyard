// sysyslog.cpp
//
// Cross-platform system logging routines for Switchyard
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved
//

#include <stdio.h>

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
  if(getenv("SWITCHYARD_SUPPRESS_DEBUG")==NULL) {
    syslog(priority,"%s",(const char *)msg.toAscii());
  }
#endif  // WIN32
}
