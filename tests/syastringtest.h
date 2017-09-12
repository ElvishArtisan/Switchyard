// syastringtest.h
//
// Test the SyAString parser
//
// (C) 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYASTRINGTEST_H
#define SYASTRINGTEST_H

#include <stdint.h>

#include <QObject>

#include <sndfile.h>

#include <sy/syconfig.h>
#include <sy/sylwrp_server.h>
#include <sy/syrouting.h>
#include <sy/syrtp_server.h>

#define SYASTRINGTEST_USAGE "<string>\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);
};


#endif  // SYASTRINGTEST_H
