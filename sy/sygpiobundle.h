// sygpiobundle.h
//
// Abstract a set of LiveWire GPIO signals
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#ifndef SYGPIOBUNDLE_H
#define SYGPIOBUNDLE_H

#include <QString>

#include <sy/syconfig.h>

class SyGpioBundle
{
 public:
  SyGpioBundle();
  QString code() const;
  void setCode(const QString &str);
  bool state(int offset) const;
  void setState(int offset,bool state);

 private:
  char gpio_code[SWITCHYARD_GPIO_BUNDLE_SIZE+1];
};


#endif  // SYGPIOBUNDLE_H
