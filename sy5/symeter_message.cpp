// sylo_meter.cpp
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

#include "symeter_message.h"

SyMeterMessage::SyMeterMessage(const QNetworkDatagram &dgram)
  : SyMessage(dgram)
{
  int16_t avg_lvls[2];
  int16_t peak_lvls[2];
  
  if((dgram.data().length()>=144)&&
     (dgram.data().left(16)=="Wheat E2 Meter 1")) {
    for(int i=0;i<slotQuantity();i++) {
      NormalizeWheatMeters(avg_lvls,peak_lvls,dgram.data(),80+i*4);
      d_avg_levels[i][SyLwrpClient::InputMeter][0]=avg_lvls[0];
      d_avg_levels[i][SyLwrpClient::InputMeter][1]=avg_lvls[1];
      d_peak_levels[i][SyLwrpClient::InputMeter][0]=peak_lvls[0];
      d_peak_levels[i][SyLwrpClient::InputMeter][1]=peak_lvls[1];
      NormalizeWheatMeters(avg_lvls,peak_lvls,dgram.data(),120+i*4);
      d_avg_levels[i][SyLwrpClient::OutputMeter][0]=avg_lvls[0];
      d_avg_levels[i][SyLwrpClient::OutputMeter][1]=avg_lvls[1];
      d_peak_levels[i][SyLwrpClient::OutputMeter][0]=peak_lvls[0];
      d_peak_levels[i][SyLwrpClient::OutputMeter][1]=peak_lvls[1];
    }
  }
}


SyMeterMessage::SyMeterMessage()
  : SyMessage()
{
  for(int i=0;i<slotQuantity();i++) {
    for(int j=0;j<2;j++) {
      for(int k=0;k<2;k++) {
	d_avg_levels[i][j][k]=-10000;
	d_peak_levels[i][j][k]=-10000;
	d_silence_alarms[i][j][k]=false;
	d_clip_alarms[i][j][k]=false;
      }
    }
  }
}


int SyMeterMessage::slotQuantity() const
{
  return 8;
}


void SyMeterMessage::meterLevels(int16_t avg_lvls[2],int16_t peak_lvls[2],
				 SyLwrpClient::MeterType mtype,int slotnum)
{
  for(int i=0;i<2;i++) {
    avg_lvls[i]=d_avg_levels[slotnum][mtype][i];
    peak_lvls[i]=d_peak_levels[slotnum][mtype][i];
  }
}


void SyMeterMessage::setMeterLevels(int16_t avg_lvls[2],int16_t peak_lvls[2],
				    SyLwrpClient::MeterType mtype,int slotnum)
{
  for(int i=0;i<2;i++) {
      d_avg_levels[slotnum][mtype][i]=avg_lvls[i];
      d_peak_levels[slotnum][mtype][i]=peak_lvls[i];
  }
}


void SyMeterMessage::audioClipAlarms(bool states[2],
				     SyLwrpClient::MeterType mtype,int slotnum)
{
  for(int i=0;i<2;i++) {
    states[i]=d_clip_alarms[slotnum][mtype][i];
  }
}


void SyMeterMessage::setAudioClipAlarms(bool states[2],
					SyLwrpClient::MeterType mtype,
					int slotnum)
{
  for(int i=0;i<2;i++) {
    d_clip_alarms[slotnum][mtype][i]=states[i];
  }
}


void SyMeterMessage::audioSilenceAlarms(bool states[2],
					SyLwrpClient::MeterType mtype,
					int slotnum)
{
  for(int i=0;i<2;i++) {
    states[i]=d_silence_alarms[slotnum][mtype][i];
  }
}


void SyMeterMessage::setAudioSilenceAlarms(bool states[2],
					   SyLwrpClient::MeterType mtype,
					   int slotnum)
{
  for(int i=0;i<2;i++) {
    d_silence_alarms[slotnum][mtype][i]=states[i];
  }
}


QString SyMeterMessage::dump()
{
  QString ret="";
  int16_t avg_lvls[2];
  int16_t peak_lvls[2];
  
  ret+=QString("hostAddress: ")+hostAddress().toString()+"\n"; 
  for(int i=0;i<slotQuantity();i++) {
    meterLevels(avg_lvls,peak_lvls,SyLwrpClient::InputMeter,i);
    ret+=QString::asprintf("Slot %d Input Levels avg: %d : %d  peak: %d : %d\n",
			   i,avg_lvls[0],avg_lvls[1],peak_lvls[0],peak_lvls[1]);

    meterLevels(avg_lvls,peak_lvls,SyLwrpClient::OutputMeter,i);
    ret+=QString::asprintf("Slot %d Output Levels avg: %d : %d  peak: %d : %d\n",
			   i,avg_lvls[0],avg_lvls[1],peak_lvls[0],peak_lvls[1]);
  }
  return ret;
}


void SyMeterMessage::NormalizeWheatMeters(int16_t *avg_lvls,int16_t *peak_lvls,
					  const char *data,
					  int offset) const
{
  for(int i=0;i<2;i++) {
    int lvl=(1200*(0xFF&data[offset+2*i])/49-6260);
    if((0xFF&data[offset+2*i])<=SWITCHYARD_WN_MIN_METER_LEVEL) {
      lvl=SWITCHYARD_MUTE_DEPTH;
    }
    avg_lvls[i]=lvl;
  }
  for(int i=0;i<2;i++) {
    int lvl=(1200*(0xFF&data[1+offset+2*i])/49-6260);
    if((0xFF&data[1+offset+2*i])<=SWITCHYARD_WN_MIN_METER_LEVEL) {
      lvl=SWITCHYARD_MUTE_DEPTH;
    }
    peak_lvls[i]=lvl;
  }
}
