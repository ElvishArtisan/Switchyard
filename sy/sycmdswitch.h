// syadv_cmdswitch.h
//
// Process Command-Line Switches
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//       All Rights Reserved.
//

#ifndef SYADV_CMDSWITCH_H
#define SYADV_CMDSWITCH_H

#include <vector>

#include <QtCore/QString>

class SyCmdSwitch
{
 public:
  SyCmdSwitch(int argc,char *argv[],const char *modname,const char *modver,
	      const char *usage);
  unsigned keys() const;
  QString key(unsigned n) const;
  QString value(unsigned n) const;
  bool processed(unsigned n) const;
  void setProcessed(unsigned n,bool state);
  bool allProcessed() const;

 private:
  std::vector<QString> switch_keys;
  std::vector<QString> switch_values;
  std::vector<bool> switch_processed;
};


#endif  // SYADV_CMDSWITCH_H
