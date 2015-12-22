// symetertest.cpp
//
// Display level meters via LWRP
//
// (C) 2015 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <QApplication>

#include <sy/sycmdswitch.h>

#include "symetertest.h"

MainWidget::MainWidget(QWidget *parent)
  : QMainWindow(parent)
{
  QString node;

  //
  // Process Command Line
  //
  SyCmdSwitch *cmd=
    new SyCmdSwitch(qApp->argc(),qApp->argv(),"symetertest",VERSION,
		    SYMETERTEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--node") {
      node=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"symetertest: unrecognized option\n");
      exit(256);
    }
  }
  if(node.isEmpty()) {
    fprintf(stderr,"symetertest: you must specify a node=<hostname>\n");
    exit(256);
  }

  //
  // Label Font
  //
  meter_label_font=QFont("helvetica",20,QFont::Bold);
  meter_label_font.setPixelSize(20);

  meter_clip_threshold_label=new QLabel(tr("Clip Threshold"),this);
  meter_clip_threshold_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  meter_clip_threshold_spin=new QSpinBox(this);
  meter_clip_threshold_spin->setRange(-100,0);
  meter_clip_threshold_spin->setSuffix(tr("dBFS"));
  connect(meter_clip_threshold_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(clipChangedData(int)));

  meter_clip_timeout_label=new QLabel(tr("Clip Timeout"),this);
  meter_clip_timeout_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  meter_clip_timeout_spin=new QSpinBox(this);
  meter_clip_timeout_spin->setRange(0,60);
  meter_clip_timeout_spin->setSuffix(tr("S"));
  connect(meter_clip_timeout_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(clipChangedData(int)));

  meter_input_label=new QLabel(tr("Input Levels"),this);
  meter_input_label->setFont(meter_label_font);
  meter_input_label->setAlignment(Qt::AlignCenter);

  meter_input_clip_label=new QLabel(tr("Clip"),this);
  meter_input_clip_label->setAlignment(Qt::AlignCenter);

  meter_input_silence_label=new QLabel(tr("Silence"),this);
  meter_input_silence_label->setAlignment(Qt::AlignCenter);


  meter_silence_threshold_label=new QLabel(tr("Silence Threshold"),this);
  meter_silence_threshold_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  meter_silence_threshold_spin=new QSpinBox(this);
  meter_silence_threshold_spin->setRange(-100,0);
  meter_silence_threshold_spin->setSuffix(tr("dBFS"));
  connect(meter_silence_threshold_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(silenceChangedData(int)));

  meter_silence_timeout_label=new QLabel(tr("Silence Timeout"),this);
  meter_silence_timeout_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  meter_silence_timeout_spin=new QSpinBox(this);
  meter_silence_timeout_spin->setRange(0,60);
  meter_silence_timeout_spin->setSuffix(tr("S"));
  connect(meter_silence_timeout_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(silenceChangedData(int)));

  meter_output_label=new QLabel(tr("Output Levels"),this);
  meter_output_label->setFont(meter_label_font);
  meter_output_label->setAlignment(Qt::AlignCenter);

  meter_output_clip_label=new QLabel(tr("Clip"),this);
  meter_output_clip_label->setAlignment(Qt::AlignCenter);

  meter_output_silence_label=new QLabel(tr("Silence"),this);
  meter_output_silence_label->setAlignment(Qt::AlignCenter);

  meter_node=new SyLwrpClient(0,this);
  connect(meter_node,SIGNAL(connected(unsigned,bool)),
	  this,SLOT(connectedData(unsigned,bool)));
  connect(meter_node,SIGNAL(meterUpdate(unsigned,SyLwrpClient::MeterType,
					unsigned,int16_t *,int16_t *)),
	  this,SLOT(meterUpdateData(unsigned,SyLwrpClient::MeterType,
				    unsigned,int16_t *,int16_t *)));
  connect(meter_node,SIGNAL(audioClipAlarm(unsigned,SyLwrpClient::MeterType,
					   unsigned,int,bool)),
	  this,SLOT(audioClipAlarmData(unsigned,SyLwrpClient::MeterType,
				       unsigned,int,bool)));
  connect(meter_node,SIGNAL(audioSilenceAlarm(unsigned,SyLwrpClient::MeterType,
					      unsigned,int,bool)),
	  this,SLOT(audioSilenceAlarmData(unsigned,SyLwrpClient::MeterType,
					  unsigned,int,bool)));

  meter_node->connectToHost(QHostAddress(node),SWITCHYARD_LWRP_PORT,"",true);

  setWindowTitle(tr("Meter Test")+" - "+node);
}


QSize MainWidget::sizeHint() const
{
  unsigned slotnum=meter_input_labels.size();
  if(meter_output_labels.size()>slotnum) {
    slotnum=meter_output_labels.size();
  }
  return QSize(1120,90+30*slotnum);
}


void MainWidget::connectedData(unsigned id,bool state)
{
  //
  // Input Meters
  //
  for(unsigned i=0;i<meter_node->srcSlots();i++) {
    meter_input_labels.push_back(new QLabel(QString().sprintf("%u",i+1),this));
    meter_input_labels.back()->setFont(meter_label_font);
    meter_input_labels.back()->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    meter_input_labels.back()->show();
  }
  for(unsigned i=0;i<(meter_node->srcSlots()*SWITCHYARD_MAX_CHANNELS);i++) {
    meter_input_meters.push_back(new PlayMeter(SegMeter::Right,this));
    meter_input_meters.back()->setRange(-1000,0);
    meter_input_meters.back()->setHighThreshold(-160);
    meter_input_meters.back()->setClipThreshold(-40);
    meter_input_meters.back()->show();
    meter_input_clip_lights.push_back(new StatusLight(this));
    meter_input_silence_lights.push_back(new StatusLight(this));
  }
  for(unsigned i=0;i<(meter_node->srcSlots()*SWITCHYARD_MAX_CHANNELS);i+=2) {
    meter_input_meters[i]->setLabel("L");
    meter_input_meters[i+1]->setLabel("R");
  }
  meter_node->startMeter(SyLwrpClient::InputMeter);

  //
  // Output Meters
  //
  for(unsigned i=0;i<meter_node->dstSlots();i++) {
    meter_output_labels.push_back(new QLabel(QString().sprintf("%u",i+1),this));
    meter_output_labels.back()->setFont(meter_label_font);
    meter_output_labels.back()->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    meter_output_labels.back()->show();
  }
  for(unsigned i=0;i<(meter_node->dstSlots()*SWITCHYARD_MAX_CHANNELS);i++) {
    meter_output_meters.push_back(new PlayMeter(SegMeter::Right,this));
    meter_output_meters.back()->setRange(-1000,0);
    meter_output_meters.back()->setHighThreshold(-160);
    meter_output_meters.back()->setClipThreshold(-40);
    meter_output_meters.back()->show();
    meter_output_clip_lights.push_back(new StatusLight(this));
    meter_output_silence_lights.push_back(new StatusLight(this));
  }
  for(unsigned i=0;i<(meter_node->dstSlots()*SWITCHYARD_MAX_CHANNELS);i+=2) {
    meter_output_meters[i]->setLabel("L");
    meter_output_meters[i+1]->setLabel("R");
  }
  meter_node->startMeter(SyLwrpClient::OutputMeter);

  show();
}


void MainWidget::meterUpdateData(unsigned id,SyLwrpClient::MeterType type,
				 unsigned slotnum,
				 int16_t *peak_lvls,int16_t *rms_lvls)
{
  switch(type) {
  case SyLwrpClient::InputMeter:
    meter_input_meters[2*slotnum]->setSolidBar(rms_lvls[0]);
    meter_input_meters[2*slotnum]->setFloatingBar(peak_lvls[0]);
    meter_input_meters[2*slotnum+1]->setSolidBar(rms_lvls[1]);
    meter_input_meters[2*slotnum+1]->setFloatingBar(peak_lvls[1]);
    break;

  case SyLwrpClient::OutputMeter:
    meter_output_meters[2*slotnum]->setSolidBar(rms_lvls[0]);
    meter_output_meters[2*slotnum]->setFloatingBar(peak_lvls[0]);
    meter_output_meters[2*slotnum+1]->setSolidBar(rms_lvls[1]);
    meter_output_meters[2*slotnum+1]->setFloatingBar(peak_lvls[1]);
    break;

  case SyLwrpClient::LastTypeMeter:
    break;
  }
  /*
  printf("METER [%u]:  %d/%d  :  %d/%d\n",
	 slotnum,peak_lvls[0],peak_lvls[1],rms_lvls[0],rms_lvls[1]);
  */
}


void MainWidget::audioClipAlarmData(unsigned id,SyLwrpClient::MeterType type,
				    unsigned slotnum,int chan,bool state)
{
  //  printf("audioClipAlarm(%u,%u,%u,%u,%u)\n",
  //	 id,type,slotnum,chan,state);
  switch(type) {
  case SyLwrpClient::InputMeter:
    meter_input_clip_lights[2*slotnum+chan]->setStatus(state);
    break;

  case SyLwrpClient::OutputMeter:
    meter_output_clip_lights[2*slotnum+chan]->setStatus(state);
    break;

  case SyLwrpClient::LastTypeMeter:
    break;
  }
}


void MainWidget::audioSilenceAlarmData(unsigned id,SyLwrpClient::MeterType type,
				       unsigned slotnum,int chan,bool state)
{
  switch(type) {
  case SyLwrpClient::InputMeter:
    meter_input_silence_lights[2*slotnum+chan]->setStatus(state);
    break;

  case SyLwrpClient::OutputMeter:
    meter_output_silence_lights[2*slotnum+chan]->setStatus(state);
    break;

  case SyLwrpClient::LastTypeMeter:
    break;
  }
}


void MainWidget::clipChangedData(int n)
{
  for(unsigned i=0;i<meter_node->srcSlots();i++) {
    meter_node->setClipMonitor(i,SyLwrpClient::InputMeter,
			       10*meter_clip_threshold_spin->value(),
			       1000*meter_clip_timeout_spin->value());
  }
  for(unsigned i=0;i<meter_node->dstSlots();i++) {
    meter_node->setClipMonitor(i,SyLwrpClient::OutputMeter,
			       10*meter_clip_threshold_spin->value(),
			       1000*meter_clip_timeout_spin->value());
  }
}


void MainWidget::silenceChangedData(int n)
{
  for(unsigned i=0;i<meter_node->srcSlots();i++) {
    meter_node->setSilenceMonitor(i,SyLwrpClient::InputMeter,
			       10*meter_silence_threshold_spin->value(),
			       1000*meter_silence_timeout_spin->value());
  }
  for(unsigned i=0;i<meter_node->dstSlots();i++) {
    meter_node->setSilenceMonitor(i,SyLwrpClient::OutputMeter,
			       10*meter_silence_threshold_spin->value(),
			       1000*meter_silence_timeout_spin->value());
  }
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  meter_clip_threshold_label->setGeometry(10,10,100,20);
  meter_clip_threshold_spin->setGeometry(115,10,100,20);

  meter_clip_timeout_label->setGeometry(310,10,100,20);
  meter_clip_timeout_spin->setGeometry(415,10,100,20);

  meter_silence_threshold_label->setGeometry(560,10,150,20);
  meter_silence_threshold_spin->setGeometry(715,10,100,20);

  meter_silence_timeout_label->setGeometry(890,10,120,20);
  meter_silence_timeout_spin->setGeometry(1015,10,100,20);

  //
  // Input Meters
  //
  meter_input_label->setGeometry(10,40,380,20);
  meter_input_clip_label->setGeometry(405,40,50,20);
  meter_input_silence_label->setGeometry(460,40,50,20);
  for(unsigned i=0;i<meter_input_meters.size();i+=SWITCHYARD_MAX_CHANNELS) {
    meter_input_labels[i/2]->setGeometry(10,65+i*15,20,20);
    for(unsigned j=0;j<SWITCHYARD_MAX_CHANNELS;j++) {
      meter_input_meters[i+j]->setGeometry(40,65+i*15+j*10,350,10);
      meter_input_clip_lights[i+j]->setGeometry(420,61+i*15+j*12,15,15);
      meter_input_silence_lights[i+j]->setGeometry(480,61+i*15+j*12,15,15);
    }
  }

  //
  // Output Meters
  //
  meter_output_label->setGeometry(610,40,380,20);
  meter_output_clip_label->setGeometry(1005,40,50,20);
  meter_output_silence_label->setGeometry(1060,40,50,20);
  for(unsigned i=0;i<meter_output_meters.size();i+=SWITCHYARD_MAX_CHANNELS) {
    meter_output_labels[i/2]->setGeometry(610,65+i*15,20,20);
    for(unsigned j=0;j<SWITCHYARD_MAX_CHANNELS;j++) {
      meter_output_meters[i+j]->setGeometry(640,65+i*15+j*10,350,10);
      meter_output_clip_lights[i+j]->setGeometry(1020,61+i*15+j*12,15,15);
      meter_output_silence_lights[i+j]->setGeometry(1080,61+i*15+j*12,15,15);
    }
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  new MainWidget();

  return a.exec();
}
