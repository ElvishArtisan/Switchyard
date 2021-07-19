// sycmdswitch.cpp
//
// Process Command-Line Switches
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>

#include <QCoreApplication>
#include <QStringList>

#include "sycmdswitch.h"

SyCmdSwitch::SyCmdSwitch(const QString &modname,const QString &modver,
			 const QString &usage)
//SyCmdSwitch::SyCmdSwitch(int argc,char *argv[],const char *modname,
//			 const char *modver,const char *usage)
{
  QStringList args=qApp->arguments();

  for(int i=1;i<args.size();i++) {
    QString value=args.at(i);
    if(value=="--version") {
      printf("%s v%s\n",modname.toUtf8().constData(),
	     modver.toUtf8().constData());
      exit(0);
    }
    if(value=="--help") {
      printf("\n%s %s\n",modname.toUtf8().constData(),
	     usage.toUtf8().constData());
      exit(0);
    }
    QStringList f0=value.split("=",QString::KeepEmptyParts);
    if(f0.size()>=2) {
      if(f0.at(0).left(1)=="-") {
	switch_keys.push_back(f0.at(0));
	for(int i=2;i<f0.size();i++) {
	  f0[1]+="="+f0.at(i);
	}
	if(f0.at(1).isEmpty()) {
	  switch_values.push_back("");
	}
	else {
	  switch_values.push_back(f0.at(1));
	}
      }
      else {
	switch_keys.push_back(f0.join("="));
	switch_values.push_back("");
      }
      switch_processed.push_back(false);
    }
    else {
      switch_keys.push_back(value);
      switch_values.push_back("");
      switch_processed.push_back(false);
    }
  }


  /*
  unsigned l=0;
  bool handled=false;

  for(int i=1;i<argc;i++) {
#ifndef WIN32
    if(!strcmp(argv[i],"--version")) {
      printf("%s v%s\n",modname,modver);
      exit(0);
    }
#endif  // WIN32
    if(!strcmp(argv[i],"--help")) {
      printf("\n%s %s\n",modname,usage);
      exit(0);
    }
    l=strlen(argv[i]);
    handled=false;
    for(unsigned j=0;j<l;j++) {
      if(argv[i][j]=='=') {
	switch_keys.push_back(QString(argv[i]).left(j));
	switch_values.push_back(QString(argv[i]).right(l-(j+1)));
	switch_processed.push_back(false);
	j=l;
	handled=true;
      }
    }
    if(!handled) {
      switch_keys.push_back(QString(argv[i]));
      switch_values.push_back(QString(""));
      switch_processed.push_back(false);
    }
  }
  */
}


int SyCmdSwitch::keys() const
{
  return switch_keys.size();
}


QString SyCmdSwitch::key(int n) const
{
  return switch_keys[n];
}


QString SyCmdSwitch::value(int n) const
{
  return switch_values[n];
}


bool SyCmdSwitch::processed(int n) const
{
  return switch_processed[n];
}


void SyCmdSwitch::setProcessed(int n,bool state)
{
  switch_processed[n]=state;
}


bool SyCmdSwitch::allProcessed() const
{
  for(int i=0;i<switch_processed.size();i++) {
    if(!switch_processed[i]) {
      return false;
    }
  }
  return true;
}
