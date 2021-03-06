// sysyslog.h
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

#ifndef SYSYSLOG_H
#define SYSYSLOG_H

#ifdef WIN32
//
// Options
//
#define LOG_CONS 0x02
#define LOG_NDELAY 0x08
#define LOG_NOWAIT 0x10
#define LOG_ODELAY 0x04
#define LOG_PERROR 0x20
#define LOG_PID 0x01

//
// Facilities
//
#define LOG_AUTH 4
#define LOG_AUTHPRIV 10
#define LOG_CRON 9
#define LOG_DAEMON 3
#define LOG_FTP 11
#define LOG_KERN 0
#define LOG_LOCAL0 16
#define LOG_LOCAL1 17
#define LOG_LOCAL2 18
#define LOG_LOCAL3 19
#define LOG_LOCAL4 20
#define LOG_LOCAL5 21
#define LOG_LOCAL6 22
#define LOG_LOCAL7 23
#define LOG_LPR 6
#define LOG_MAIL 2
#define LOG_NEWS 7
#define LOG_SYSLOG 5
#define LOG_USER 1
#define LOG_UUCP 8

//
// Levels
//
#define LOG_EMERG 0
#define LOG_ALERT 1
#define LOG_CRIT 2
#define LOG_ERR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

#else
#include <syslog.h>
#endif  // WIN32

#include <QtCore/QString>

void SyOpenLog(const QString &ident,int option,int facility);
void SySyslog(int priority,const QString &msg);


#endif  // SYSYSLOG_H
