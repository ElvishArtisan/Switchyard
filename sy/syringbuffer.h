// syringbuffer.h
//
// A ringbuffer class for PCM audio
//
// (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYRINGBUFFER_H
#define SYRINGBUFFER_H

#include <sys/types.h>

class SyRingbuffer
{
 public:
  SyRingbuffer(unsigned frames,unsigned channels);
  ~SyRingbuffer();
  unsigned size() const;
  unsigned read(float *data,unsigned frames);
  unsigned readSpace() const;
  unsigned write(float *data,unsigned frames);
  unsigned writeSpace() const;

 private:
  void CopyFrames(float *dst,const float *src,unsigned frames) const;
  float *ring_buffer;
  unsigned ring_write_ptr;
  unsigned ring_read_ptr;
  unsigned ring_size;
  unsigned ring_mask;
  unsigned ring_channels;
};


#endif  // SYRINGBUFFER_H
