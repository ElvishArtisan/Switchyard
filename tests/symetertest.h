// symetertest.h
//
// Display level meters via LWRP
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#ifndef SYMETERTEST_H
#define SYMETERTEST_H

#include <vector>

#include <QLabel>
#include <QMainWindow>
#include <QSpinBox>

#include <sy/sylwrp_client.h>

#include "playmeter.h"
#include "statuslight.h"

#define SYMETERTEST_USAGE "--node=<hostname>\n"

class MainWidget : public QMainWindow
{
  Q_OBJECT;
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void connectedData(unsigned id,bool state);
  void meterUpdateData(unsigned id,SyLwrpClient::MeterType type,
		       unsigned slotnum,int16_t *peak_lvls,int16_t *rms_lvls);
  void audioClipAlarmData(unsigned id,SyLwrpClient::MeterType type,
			  unsigned slotnum,int chan,bool state);
  void audioSilenceAlarmData(unsigned id,SyLwrpClient::MeterType type,
			     unsigned slotnum,int chan,bool state);
  void clipChangedData(int n);
  void silenceChangedData(int n);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  SyLwrpClient *meter_node;
  QLabel *meter_input_label;
  QLabel *meter_input_clip_label;
  QLabel *meter_input_silence_label;
  std::vector<QLabel *> meter_input_labels;
  std::vector<PlayMeter *> meter_input_meters;
  std::vector<StatusLight *> meter_input_clip_lights;
  std::vector<StatusLight *> meter_input_silence_lights;
  QLabel *meter_output_label;
  QLabel *meter_output_clip_label;
  QLabel *meter_output_silence_label;
  std::vector<QLabel *> meter_output_labels;
  std::vector<PlayMeter *> meter_output_meters;
  std::vector<StatusLight *> meter_output_clip_lights;
  std::vector<StatusLight *> meter_output_silence_lights;
  QFont meter_label_font;
  QLabel *meter_clip_threshold_label;
  QSpinBox *meter_clip_threshold_spin;
  QLabel *meter_clip_timeout_label;
  QSpinBox *meter_clip_timeout_spin;
  QLabel *meter_silence_threshold_label;
  QSpinBox *meter_silence_threshold_spin;
  QLabel *meter_silence_timeout_label;
  QSpinBox *meter_silence_timeout_spin;
};


#endif  // SYMETERTEST_H
