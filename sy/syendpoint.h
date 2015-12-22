// syendpoint.h
//
// Container class for LiveWire audio endpoint attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
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
