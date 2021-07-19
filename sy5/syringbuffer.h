// syringbuffer.h
//
// A ringbuffer class for PCM audio
//
// (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
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
