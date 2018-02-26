// syadv_cmdswitch.h
//
// Process Command-Line Switches
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYADV_CMDSWITCH_H
#define SYADV_CMDSWITCH_H

#include <vector>

#include <QString>

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
