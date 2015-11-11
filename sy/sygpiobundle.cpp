// sygpiobundle.cpp
//
// Abstract a set of LiveWire GPIO signals
//
// (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   All Rights Reserved.
//

#include <string.h>

#include "sygpiobundle.h"

SyGpioBundle::SyGpioBundle()
{
  strcpy(gpio_code,"hhhhh");
}


QString SyGpioBundle::code() const
{
  return gpio_code;
}


void SyGpioBundle::setCode(const QString &str)
{
  strncpy(gpio_code,str.toAscii(),SWITCHYARD_GPIO_BUNDLE_SIZE);
}


bool SyGpioBundle::state(int offset) const
{
  return (gpio_code[offset]=='h')||(gpio_code[offset]=='H');
}


void SyGpioBundle::setState(int offset,bool state)
{
  if(state) {
    gpio_code[offset]='h';
  }
  else {
    gpio_code[offset]='l';
  }
}
