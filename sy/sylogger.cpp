// sylogger.cpp
//
// Cross-platform logging for the Switchyard
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved
//

#include <stdarg.h>

#include "sylogger.h"

void SyOpenLog(const QString &ident)
{
#ifdef LINUX
  openlog(ident.toAscii(),LOG_NDELAY|LOG_PERROR,LOG_DAEMON);
#endif  // LINUX
#ifdef OSX
  openlog(ident.toAscii(),LOG_NDELAY|LOG_PERROR,LOG_DAEMON);
#endif  // OSX
}


void SySyslog(int priority,const QString &msg)
{
#ifdef LINUX
  syslog(priority,"%s",(const char *)msg.toUtf8());
#endif  // LINUX
#ifdef OSX
  syslog(priority,"%s",(const char *)msg.toUtf8());
#endif  // OSX
}


void SyCloseLog(void)
{
#ifdef LINUX
  closelog();
#endif  // LINUX
#ifdef OSX
  closelog();
#endif  // OSX
}
