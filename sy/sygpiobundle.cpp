// sygpiobundle.cpp
//
// Abstract a set of LiveWire GPIO signals
//
// (C) Copyright 2015-2021 Fred Gleason <fredg@paravelsystems.com>
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
  strncpy(gpio_code,str.toUtf8(),SWITCHYARD_GPIO_BUNDLE_SIZE);
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


QString SyGpioBundle::invertCode(const QString &code)
{
  QString ret="";

  for(int i=0;i<code.length();i++) {
    QChar c=code.at(i);
    if(c=='H') {
      ret+='L';
    }
    else {
      if(c=='h') {
	ret+='l';
      }
      else {
	if(c=='L') {
	  ret+='H';
	}
	else {
	  if(c=='l') {
	    ret+='h';
	  }
	  else {
	    ret+=c;
	  }
	}
      }
    }
  }

  return ret;
}
