// syendpoint.h
//
// Container class for LiveWire audio endpoint attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYENDPOINT_H
#define SYENDPOINT_H

class SyEndPoint
{
 public:
  SyEndPoint();
  int clipThreshold() const;
  void setClipThreshold(int lvl);
  int clipTimeout() const;
  void setClipTimeout(int msec);
  int silenceThreshold() const;
  void setSilenceThreshold(int lvl);
  int silenceTimeout() const;
  void setSilenceTimeout(int msec);

 private:
  int ep_clip_threshold;
  int ep_clip_timeout;
  int ep_silence_threshold;
  int ep_silence_timeout;
};


#endif  // SYENDPOINT_H
