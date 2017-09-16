// interfacestest.h
//
// Test the SyInterfaces class.
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef INTERFACESTEST_H
#define INTERFACESTEST_H

#include <QObject>

#include <sy/syinterfaces.h>

#define INTERFACESTEST_USAGE "\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private:
  SyInterfaces *test_interfaces;
};


#endif  // INTERFACESTEST_H
