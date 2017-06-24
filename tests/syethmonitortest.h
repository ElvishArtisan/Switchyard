// syethmonitortest.h
//
// Monitor a network interface and report changes in state.
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYETHMONITORTEST_H
#define SYETHMONITORTEST_H

#include <QObject>

#include <sy/syethmonitor.h>

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
