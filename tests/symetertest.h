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

#include <sy/sylwrp_client.h>

#include "playmeter.h"

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

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  SyLwrpClient *meter_node;
  QLabel *meter_input_label;
  std::vector<QLabel *> meter_input_labels;
  std::vector<PlayMeter *> meter_input_meters;
  QLabel *meter_output_label;
  std::vector<QLabel *> meter_output_labels;
  std::vector<PlayMeter *> meter_output_meters;
  QFont meter_label_font;
};


#endif  // SYMETERTEST_H
