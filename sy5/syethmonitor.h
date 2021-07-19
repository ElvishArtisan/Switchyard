// syethmonitor.h
//
// Monitor a physical Ethernet interface
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYETHMONITOR_H
#define SYETHMONITOR_H

#include <net/if.h>
#include <sys/ioctl.h>

#include <QObject>
#include <QTimer>

#define SYETHMONITOR_MONITOR_INTERVAL 100

class SyEthMonitor :public QObject
{
  Q_OBJECT;
 public:
  SyEthMonitor(const QString &iface,QObject *parent=0);
  ~SyEthMonitor();
  QString interfaceName() const;
  bool isRunning() const;

 signals:
  void startedRunning();
  void stoppedRunning();

 private slots:
  void timeoutData();

 private:
  int eth_socket;
  QTimer *eth_timer;
  bool eth_is_running;
  QString eth_interface_name;
  struct ifreq eth_ifreq;
};


#endif  // SYETHMONITOR_H
