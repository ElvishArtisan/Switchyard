// sysignalnotifier.h
//
// Notifications for UNIX-style signals
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYSIGNALNOTIFIER_H
#define SYSIGNALNOTIFIER_H

#include <QList>
#include <QObject>
#include <QSocketNotifier>

class SySignalNotifier : public QObject
{
  Q_OBJECT;
 public:
  SySignalNotifier(QObject *parent=0);
  ~SySignalNotifier();
  QList<int> monitoredSignals() const;
  bool addSignal(int signum);
  bool removeSignal(int signum);

 private slots:
   //  void socketActivatedData(QSocketDescriptor sock,QSocketNotifier::Type type);
  void socketActivatedData(int sock);
 
 signals:
  void activated(int signum);

 private:
  QList<int> d_signal_numbers;
  QSocketNotifier *d_ipc_notifier;
};


#endif  // SYSIGNALNOTIFIER_H
