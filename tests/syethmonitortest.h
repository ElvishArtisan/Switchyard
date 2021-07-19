// syethmonitortest.h
//
// Monitor a network interface and report changes in state.
//
// (C) 2017-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYETHMONITORTEST_H
#define SYETHMONITORTEST_H

#include <QObject>

#include <sy5/syethmonitor.h>

#define SYETHMONITORTEST_USAGE "--iface=<iface>\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void startedRunningData();
  void stoppedRunningData();

 private:
  SyEthMonitor *test_monitor;
};


#endif  // SYETHMONITORTEST_H
