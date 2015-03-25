// sycmdswitch.cpp
//
// Process Command-Line Switches
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//       All Rights Reserved
//

#include <stdlib.h>
#include <stdio.h>

#include "sycmdswitch.h"

SyCmdSwitch::SyCmdSwitch(int argc,char *argv[],const char *modname,
			 const char *modver,const char *usage)
{
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
}


unsigned SyCmdSwitch::keys() const
{
  return switch_keys.size();
}


QString SyCmdSwitch::key(unsigned n) const
{
  return switch_keys[n];
}


QString SyCmdSwitch::value(unsigned n) const
{
  return switch_values[n];
}


bool SyCmdSwitch::processed(unsigned n) const
{
  return switch_processed[n];
}


void SyCmdSwitch::setProcessed(unsigned n,bool state)
{
  switch_processed[n]=state;
}


bool SyCmdSwitch::allProcessed() const
{
  for(unsigned i=0;i<switch_processed.size();i++) {
    if(!switch_processed[i]) {
      return false;
    }
  }
  return true;
}
