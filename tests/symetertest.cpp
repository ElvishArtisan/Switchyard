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

  meter_input_label=new QLabel(tr("Input Levels"),this);
  meter_input_label->setFont(meter_label_font);
  meter_input_label->setAlignment(Qt::AlignCenter);

  meter_output_label=new QLabel(tr("Output Levels"),this);
  meter_output_label->setFont(meter_label_font);
  meter_output_label->setAlignment(Qt::AlignCenter);

  meter_node=new SyLwrpClient(0,this);
  connect(meter_node,SIGNAL(connected(unsigned,bool)),
	  this,SLOT(connectedData(unsigned,bool)));
  connect(meter_node,SIGNAL(meterUpdate(unsigned,SyLwrpClient::MeterType,
					unsigned,int16_t *,int16_t *)),
	  this,SLOT(meterUpdateData(unsigned,SyLwrpClient::MeterType,
				    unsigned,int16_t *,int16_t *)));
  meter_node->connectToHost(QHostAddress(node),SWITCHYARD_LWRP_PORT,"",true);

  setWindowTitle(tr("Meter Test")+" - "+node);
}


QSize MainWidget::sizeHint() const
{
  unsigned slotnum=meter_input_labels.size();
  if(meter_output_labels.size()>slotnum) {
    slotnum=meter_output_labels.size();
  }
  return QSize(810,60+20*slotnum);
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


void MainWidget::resizeEvent(QResizeEvent *e)
{
  //
  // Input Meters
  //
  meter_input_label->setGeometry(10,10,380,20);
  for(unsigned i=0;i<meter_input_meters.size();i+=SWITCHYARD_MAX_CHANNELS) {
    meter_input_labels[i/2]->setGeometry(10,35+i*11,20,20);
    for(unsigned j=0;j<SWITCHYARD_MAX_CHANNELS;j++) {
      meter_input_meters[i+j]->setGeometry(40,35+i*11+j*10,350,10);
    }
  }

  //
  // Output Meters
  //
  meter_output_label->setGeometry(410,10,380,20);
  for(unsigned i=0;i<meter_output_meters.size();i+=SWITCHYARD_MAX_CHANNELS) {
    meter_output_labels[i/2]->setGeometry(410,35+i*11,20,20);
    for(unsigned j=0;j<SWITCHYARD_MAX_CHANNELS;j++) {
      meter_output_meters[i+j]->setGeometry(440,35+i*11+j*10,350,10);
    }
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  new MainWidget();

  return a.exec();
}
