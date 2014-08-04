// sylogger.h
//
// Cross-platform logging for the Switchyard
//
// (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved
//

#ifndef SYLOGGER_H
#define SYLOGGER_H

#ifdef LINUX
#include <syslog.h>
#endif  // LINUX
#ifdef OSX
#include <syslog.h>
#endif  // OSX

#include <QtCore/QString>

void SyOpenLog(const QString &ident);
void SySyslog(int priority,const QString &msg);
void SyCloseLog(void);


#endif  // SYLOGGER_H
