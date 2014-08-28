// syrtp_header.h
//
// (C) Copyright 2008-2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//
// A container class for RTP header data.
//
// For information on these data structures, see RFCs 3550 and 3551.
//

#ifndef SYRTP_HEADER_H
#define SYRTP_HEADER_H

#include <stdint.h>
#include <sys/types.h>
#include <vector>
#ifdef WIN32
//#include <win32_types.h>
#include <Winsock2.h>
#endif  // WIN32

#define RTPHEADER_MIN_SIZE 8
#define RTPHEADER_MAX_CSRCS 15
#define RTPHEADER_MAX_SIZE 72

class SyRtpHeader
{
 public:
  enum PayloadType {PayloadPcmu=0,PayloadGsm=3,PayloadG723=4,PayloadDvi4_8000=5,
		    PayloadDvi4_16000=6,PayloadLpc=7,PayloadPcma=8,
		    PayloadG722=9,PayloadL16_2=10,PayloadL16_1=11,
		    PayloadQcelp=12,PayloadCn=13,PayloadMpa=14,PayloadG728=15,
		    PayloadDvi4_11025=16,PayloadDvi4_22050=17,PayloadG729=18,
		    PayloadCelB=25,PayloadJpeg=26,PayloadNv=28,PayloadH261=31,
		    PayloadMpv=32,PayloadMp2t=33,PayloadH263=34,
		    PayloadDynamicFirst=96,PayloadDynamicLast=127};
  SyRtpHeader(SyRtpHeader::PayloadType type,unsigned version=2);
  unsigned version() const;
  bool paddingBit() const;
  void setPaddingBit(bool state);
  bool extensionBit() const;
  void setExtensionBit(bool state);
  bool markerBit() const;
  void setMarkerBit(bool state);
  SyRtpHeader::PayloadType payloadType() const;
  void setPayloadType(SyRtpHeader::PayloadType type);
  uint16_t sequenceNumber() const;
  void setSequenceNumber(uint16_t num);
  uint32_t timestamp() const;
  void setTimestamp(uint32_t stamp);
  unsigned timestampInterval() const;
  void setTimestampInterval(unsigned interval);
  uint32_t ssrc() const;
  void setSsrc(uint32_t ssrc);
  unsigned csrcCount() const;
  uint32_t csrc(unsigned n) const;
  bool addCsrc(uint32_t csrc);
  void removeCsrc(unsigned n);
  SyRtpHeader &operator++();
  SyRtpHeader operator++(int);
  int readBlock(uint32_t *data,unsigned len);
  int writeBlock(uint32_t *data,unsigned len) const;
  void clear();

 private:
  unsigned rtp_version;
  bool rtp_padding_bit;
  bool rtp_extension_bit;
  bool rtp_marker_bit;
  SyRtpHeader::PayloadType rtp_payload_type;
  uint16_t rtp_sequence_number;
  uint32_t rtp_timestamp;
  unsigned rtp_timestamp_interval;
  uint32_t rtp_ssrc;
  unsigned rtp_random_seedp;
  std::vector<uint32_t> rtp_csrcs;
};


#endif  // SYRTP_HEADER_H
