// syendpoint.cpp
//
// Container class for LiveWire audio endpoint attributes
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include "syendpoint.h"

SyEndPoint::SyEndPoint()
{
  ep_clip_threshold=0;
  ep_clip_timeout=0;
  ep_silence_threshold=0;
  ep_silence_timeout=0;
}


int SyEndPoint::clipThreshold() const
{
  return ep_clip_threshold;
}


void SyEndPoint::setClipThreshold(int lvl)
{
  ep_clip_threshold=lvl;
}


int SyEndPoint::clipTimeout() const
{
  return ep_clip_timeout;
}


void SyEndPoint::setClipTimeout(int msec)
{
  ep_clip_timeout=msec;
}


int SyEndPoint::silenceThreshold() const
{
  return ep_silence_threshold;
}


void SyEndPoint::setSilenceThreshold(int lvl)
{
  ep_silence_threshold=lvl;
}


int SyEndPoint::silenceTimeout() const
{
  return ep_silence_timeout;
}


void SyEndPoint::setSilenceTimeout(int msec)
{
  ep_silence_timeout=msec;
}
