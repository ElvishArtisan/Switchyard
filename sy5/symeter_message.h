// symeter_message.h
//
// Abstract a WheatNet Meter Message
//
// (C) Copyright 2026 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SYMETER_MESSAGE_H
#define SYMETER_MESSAGE_H

#include <QDateTime>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QString>

#include <sy5/sylwrp_client.h>
#include <sy5/symessage.h>

class SyMeterMessage : public SyMessage
{
 public:
  SyMeterMessage(const QNetworkDatagram &dgram);
  SyMeterMessage();
  virtual ~SyMeterMessage()=default;
  int slotQuantity() const;
  void meterLevels(int16_t avg_lvls[2],int16_t peak_lvls[2],
		   SyLwrpClient::MeterType mtype,int slotnum);
  void setMeterLevels(int16_t avg_lvls[2],int16_t peak_lvls[2],
		      SyLwrpClient::MeterType mtype,int slotnum);
  void audioClipAlarms(bool states[2],SyLwrpClient::MeterType mtype,
		       int slotnum);
  void setAudioClipAlarms(bool states[2],SyLwrpClient::MeterType mtype,
			  int slotnum);
  void audioSilenceAlarms(bool states[2],SyLwrpClient::MeterType mtype,
			  int slotnum);
  void setAudioSilenceAlarms(bool states[2],SyLwrpClient::MeterType mtype,
			     int slotnum);
  QString dump();

 private:
  void NormalizeWheatMeters(int16_t *avg_lvls,int16_t *peak_lvls,
			    const char *data,int offset) const;
  int16_t d_avg_levels[SWITCHYARD_MAX_SLOTS][SyLwrpClient::LastTypeMeter][2];
  int16_t d_peak_levels[SWITCHYARD_MAX_SLOTS][SyLwrpClient::LastTypeMeter][2];
  bool d_clip_alarms[SWITCHYARD_MAX_SLOTS][SyLwrpClient::LastTypeMeter][2];
  bool d_silence_alarms[SWITCHYARD_MAX_SLOTS][SyLwrpClient::LastTypeMeter][2];
};


#endif  // SYMETER_MESSAGE_H
