// syethmonitor.h
//
// Monitor a physical Ethernet interface
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
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
