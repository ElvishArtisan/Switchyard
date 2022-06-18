// playmeter.cpp
//
// This implements a widget that represents a stereo audio level meter,
// complete with labels and scale.
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>

#include <QPainter>

#include "playmeter.h"

SegMeter::SegMeter(SegMeter::Orientation o,QWidget *parent)
  : QWidget(parent)
{
  orient=o;
  dark_low_color=QColor(DEFAULT_DARK_LOW_COLOR);
  dark_high_color=QColor(DEFAULT_DARK_HIGH_COLOR);
  dark_clip_color=QColor(DEFAULT_DARK_CLIP_COLOR);
  low_color=QColor(DEFAULT_LOW_COLOR);
  high_color=QColor(DEFAULT_HIGH_COLOR);
  clip_color=QColor(DEFAULT_CLIP_COLOR);
  high_threshold=-14;
  clip_threshold=0;
  seg_size=2;
  seg_gap=1;
  range_min=-3000;
  range_max=0;
  solid_bar=-10000;
  floating_bar=-10000;
  seg_mode=SegMeter::Independent;

  peak_timer=new QTimer(this);
  connect(peak_timer,SIGNAL(timeout()),this,SLOT(peakData()));
}


QSize SegMeter::sizeHint() const
{
  return QSize(0,0);
}


QSizePolicy SegMeter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void SegMeter::setRange(int min,int max)
{
  range_min=min;
  range_max=max;
  repaint();
}


void SegMeter::setDarkLowColor(QColor color)
{
  if(dark_low_color!=color) {
    dark_low_color=color;
    repaint();
  }
}


void SegMeter::setDarkHighColor(QColor color)
{
  if(dark_high_color!=color) {
    dark_high_color=color;
    repaint();
  }
}


void SegMeter::setDarkClipColor(QColor color)
{
  if(dark_clip_color!=color) {
    dark_clip_color=color;
    repaint();
  }
}


void SegMeter::setLowColor(QColor color)
{
  if(low_color!=color) {
    low_color=color;
    repaint();
  }
}


void SegMeter::setHighColor(QColor color)
{
  if(high_color!=color) {
    high_color=color;
    repaint();
  }
}


void SegMeter::setClipColor(QColor color)
{
  if(clip_color!=color) {
    clip_color=color;
    repaint();
  }
}


void SegMeter::setHighThreshold(int level)
{
  if(high_threshold!=level) {
    high_threshold=level;
    repaint();
  }
}


void SegMeter::setClipThreshold(int level)
{
  if(clip_threshold!=level) {
    clip_threshold=level;
    repaint();
  }
}


SegMeter::Mode SegMeter::mode() const
{
  return seg_mode;
}


void SegMeter::setMode(SegMeter::Mode mode)
{
  seg_mode=mode;
  switch(seg_mode) {
      case SegMeter::Independent:
	if(peak_timer->isActive()) {
	  peak_timer->stop();
	}
	break;
      case SegMeter::Peak:
	if(!peak_timer->isActive()) {
	  peak_timer->start(PEAK_HOLD_TIME);
	}
	break;
  }
}


void SegMeter::setSolidBar(int level)
{
  if((seg_mode==SegMeter::Independent)&&(solid_bar!=level)) {
    solid_bar=level;
    repaint();
  }
}


void SegMeter::setFloatingBar(int level)
{
  if((seg_mode==SegMeter::Independent)&&(solid_bar!=level)) {
    floating_bar=level;
    repaint();
  }
}


void SegMeter::setPeakBar(int level)
{
  if((seg_mode==SegMeter::Peak)&&(solid_bar!=level)) {
    solid_bar=level;
    if(solid_bar>floating_bar) {
      floating_bar=solid_bar;
    }

    if(solid_bar<range_min) {
      floating_bar=solid_bar;
    }
    repaint();
  }
}


void SegMeter::setSegmentSize(int size)
{
  if(seg_size!=size) {
    seg_size=size;
    repaint();
  }
}


void SegMeter::setSegmentGap(int gap)
{
  if(seg_gap!=gap) {
    seg_gap=gap;
    repaint();
  }
}


void SegMeter::paintEvent(QPaintEvent *paintEvent)
{
  int op_pt;
  int low_region,high_region,clip_region,float_region;
  int dark_low_region=0;
  int dark_high_region=0;
  int dark_clip_region=0;
  QColor float_color;

  //
  // Setup
  //
  QPixmap pix(this->size());
  pix.fill(Qt::black);

  int seg_total=seg_size+seg_gap;
  QPainter *p=new QPainter(&pix);
  low_region=0;
  high_region=0;
  clip_region=0;
  p->setBrush(low_color);
  p->setPen(low_color);

  //
  // Set Orientation
  //
  switch(orient) {
      case SegMeter::Left:
      case SegMeter::Up:
	p->translate(width(),height());
	p->rotate(180);
	break;

      default:
	break;
  }

  // 
  // The low range
  //
  if(solid_bar>high_threshold) {
    op_pt=high_threshold;
  }
  else {
    op_pt=solid_bar;
  }
  switch(orient) {
      case SegMeter::Left:
      case SegMeter::Right:
	low_region=(int)((double)(op_pt-range_min)/
			 (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>range_min) {
	  for(int i=0;i<low_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),low_color);
	  }
	}
	break;
      case SegMeter::Down:
      case SegMeter::Up:
	low_region=(int)((double)(op_pt-range_min)/
			 (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>range_min) {
	  for(int i=0;i<low_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,low_color);
	  }
	}
	break;
  }

  // 
  // The high range
  //
  if(solid_bar>clip_threshold) {
    op_pt=clip_threshold;
  }
  else {
    op_pt=solid_bar;
  }
  switch(orient) {
      case SegMeter::Left:
      case SegMeter::Right:
	high_region=(int)((double)(op_pt-high_threshold)/
			  (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>high_threshold) {
	  for(int i=low_region;i<low_region+high_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),high_color);
	  }
	}
	break;
      case SegMeter::Down:
      case SegMeter::Up:
	high_region=(int)((double)(op_pt-high_threshold)/
			  (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>high_threshold) {
	  for(int i=low_region;i<low_region+high_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,high_color);
	  }
	}
	break;
  }

  // 
  // The clip range
  //
  if(solid_bar>range_max) {
    op_pt=range_max;
  }
  else {
    op_pt=solid_bar;
  }
  switch(orient) {
      case SegMeter::Left:
      case SegMeter::Right:
	clip_region=(int)((double)(op_pt-clip_threshold)/
			  (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>clip_threshold) {
	  for(int i=low_region+high_region;
	      i<low_region+high_region+clip_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),clip_color);
	  }
	}
	break;
      case SegMeter::Down:
      case SegMeter::Up:
	clip_region=(int)((double)(op_pt-range_min)/
			  (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>clip_threshold) {
	  for(int i=low_region+high_region;
	      i<low_region+high_region+clip_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,clip_color);
	  }
	}
	break;
  }

  // 
  // The dark low range
  //
  switch(orient) {
      case SegMeter::Left:
      case SegMeter::Right:
	dark_low_region=(int)((double)(high_threshold-range_min)/
			      (double)(range_max-range_min)*width()/seg_total);
	if(op_pt<high_threshold) {
	  for(int i=low_region;i<dark_low_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),dark_low_color);
	  }
	}
	break;
      case SegMeter::Down:
      case SegMeter::Up:
	dark_low_region=(int)((double)(high_threshold-range_min)/
		      (double)(range_max-range_min)*height()/seg_total);
	if(op_pt<high_threshold) {
	  for(int i=low_region;i<dark_low_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_low_color);
	  }
	}
	break;
  }

  // 
  // The dark high range
  //
  if(solid_bar>=high_threshold) {
    op_pt=low_region+high_region;
  }
  else {
    op_pt=dark_low_region;
  }
  switch(orient) {
      case SegMeter::Left:
	case SegMeter::Right:
	  dark_high_region=(int)((double)(clip_threshold-range_min)/
			      (double)(range_max-range_min)*width()/seg_total);
	  if(solid_bar<clip_threshold) {
	    for(int i=op_pt;
		i<dark_high_region;i++) {
	      p->fillRect(i*seg_total,0,seg_size,height(),dark_high_color);
	    }
	  }
	  break;
      case SegMeter::Down:
      case SegMeter::Up:
	dark_high_region=(int)((double)(clip_threshold-range_min)/
		       (double)(range_max-range_min)*height()/seg_total);
	if(solid_bar<clip_threshold) {
	  for(int i=op_pt;i<dark_high_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_high_color);
	  }
	}
	break;
  }

  // 
  // The dark clip range
  //
  if(solid_bar>clip_threshold) {
    op_pt=low_region+high_region+clip_region;
  }
  else {
    op_pt=dark_high_region;
  }
  switch(orient) {
      case SegMeter::Left:
      case SegMeter::Right:
	dark_clip_region=(int)((double)(range_max-range_min)/
       		       (double)(range_max-range_min)*width()/seg_total);
	if(solid_bar<range_max) {
	  for(int i=op_pt;i<dark_clip_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),dark_clip_color);
	  }
	}
	break;
      case SegMeter::Down:
      case SegMeter::Up:
	dark_clip_region=(int)((double)(range_max-range_min)/
			       (double)(range_max-range_min)*height()/seg_total);
	if(solid_bar<range_max) {
	  for(int i=op_pt;
	      i<dark_clip_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_clip_color);
	  }
	}
	break;
  }

  //
  // The floating segment
  //
  if(floating_bar>solid_bar) {
    if(floating_bar<=high_threshold) {
      float_color=low_color;
    }
    if((floating_bar>high_threshold)&&(floating_bar<=clip_threshold)) {
      float_color=high_color;
    }
    if(floating_bar>clip_threshold) {
      float_color=clip_color;
    }
    switch(orient) {
	case SegMeter::Left:
	case SegMeter::Right:
	  float_region=(int)((double)(floating_bar-range_min)/
			     (double)(range_max-range_min)*width());
	  float_region=seg_total*(float_region/seg_total);
	  p->fillRect(float_region,0,seg_size,height(),float_color); 
	  break;

	case SegMeter::Down:
	case SegMeter::Up:
	  float_region=(int)((double)(floating_bar-range_min)/
			     (double)(range_max-range_min)*height());
	  float_region=seg_total*(float_region/seg_total);
	  p->fillRect(0,float_region,width(),seg_size,float_color); 
	  break;
    }
  } 

  p->end();
  p->begin(this);
  p->drawPixmap(0,0,pix);
  p->end();
  delete p;
}


void SegMeter::peakData()
{
  floating_bar=solid_bar;
  repaint();
}


PlayMeter::PlayMeter(SegMeter::Orientation orient,QWidget *parent)
  : QWidget(parent)
{
  meter_label=QString("");
  orientation=orient;
  makeFont();
  meter=new SegMeter(orientation,this);
  meter->setSegmentSize(5);
  meter->setSegmentGap(1);
}


QSize PlayMeter::sizeHint() const
{
  if(meter_label==QString("")) {
    return QSize(335,60);
  }
  else {
    return QSize(335,80);
  }
}


QSizePolicy PlayMeter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}


void PlayMeter::setRange(int min,int max)
{
  meter->setRange(min,max);
}


void PlayMeter::setDarkLowColor(QColor color)
{
  meter->setDarkLowColor(color);
}


void PlayMeter::setDarkHighColor(QColor color)
{
  meter->setDarkHighColor(color);
}


void PlayMeter::setDarkClipColor(QColor color)
{
  meter->setDarkClipColor(color);
}


void PlayMeter::setLowColor(QColor color)
{
  meter->setLowColor(color);
}


void PlayMeter::setHighColor(QColor color)
{
  meter->setHighColor(color);
}


void PlayMeter::setClipColor(QColor color)
{
  meter->setClipColor(color);
}


void PlayMeter::setHighThreshold(int level)
{
  meter->setHighThreshold(level);
}


void PlayMeter::setClipThreshold(int level)
{
  meter->setClipThreshold(level);
}


void PlayMeter::setLabel(QString label)
{
  meter_label=label;
  makeFont();
  setGeometry(geometry().left(),geometry().top(),
	      geometry().width(),geometry().height());
}


void PlayMeter::setGeometry(int x,int y,int w,int h)
{
  QWidget::setGeometry(x,y,w,h);
  if(meter_label.isEmpty()) {
    meter->setGeometry(2,2,w-4,h-4);
  }
  else {
    switch(orientation) {
	case SegMeter::Left:
	  meter->setGeometry(2,2,w-4-h,h-4);
	  label_font=QFont("helvetica",height()-2,QFont::Bold);
	  label_font.setPixelSize(height()-2);
	  break;
	case SegMeter::Right:
	  meter->setGeometry(2+h,2,w-4-h,h-4);
	  label_font=QFont("helvetica",height()-2,QFont::Bold);
	  label_font.setPixelSize(height()-2);
	  break;
	case SegMeter::Up:
	  meter->setGeometry(2,2,w-4,h-4-w);
	  label_font=QFont("helvetica",width()-2,QFont::Bold);
	  label_font.setPixelSize(width()-2);
	  break;
	case SegMeter::Down:
	  meter->setGeometry(2,2+width(),w-4,h-4-w);
	  label_font=QFont("helvetica",width()-2,QFont::Bold);
	  label_font.setPixelSize(width()-2);
	  break;
    }
    makeFont();
  }
}


void PlayMeter::setGeometry(QRect &rect)
{
  setGeometry(rect.left(),rect.top(),rect.width(),rect.height());
}


void PlayMeter::setSolidBar(int level)
{
  meter->setSolidBar(level);
}


void PlayMeter::setPeakBar(int level)
{
  meter->setPeakBar(level);
}


void PlayMeter::setFloatingBar(int level)
{
  meter->setFloatingBar(level);
}


void PlayMeter::setSegmentSize(int size)
{
  meter->setSegmentSize(size);
}


void PlayMeter::setSegmentGap(int gap)
{
  meter->setSegmentGap(gap);
}


SegMeter::Mode PlayMeter::mode() const
{
  return meter->mode();
}


void PlayMeter::setMode(SegMeter::Mode mode)
{
  meter->setMode(mode);
}


void PlayMeter::paintEvent(QPaintEvent *paintEvent)
{
  //
  // Setup
  //
  QPainter *p=new QPainter(this);
  p->fillRect(0,0,size().width(),size().height(),Qt::black);
  p->setFont(label_font);
  p->setPen(Qt::white);
  if(!meter_label.isEmpty()) {
    switch(orientation) {
	case SegMeter::Left:
	  p->drawText(width()-height()+meter_label_x,height()-2,meter_label);
	  break;
	case SegMeter::Right:
	  p->drawText(meter_label_x,height()-2,meter_label);
	  break;
	case SegMeter::Up:
	  p->drawText(meter_label_x,height()-3,meter_label);
	  break;
	case SegMeter::Down:
	  p->drawText(meter_label_x,width()-1,meter_label);
	  break;
    }
  }
  p->end();
}


void PlayMeter::makeFont()
{
  switch(orientation) {
      case SegMeter::Left:
	label_font=QFont("helvetica",height()-2,QFont::Bold);
	label_font.setPixelSize(height()-2);
	meter_label_x=(height()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case SegMeter::Right:
	label_font=QFont("helvetica",height()-2,QFont::Bold);
	label_font.setPixelSize(height()-2);
	meter_label_x=(height()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case SegMeter::Up:
	label_font=QFont("helvetica",width()-2,QFont::Bold);
	label_font.setPixelSize(width()-2);
	meter_label_x=(width()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case SegMeter::Down:
	label_font=QFont("helvetica",width()-2,QFont::Bold);
	label_font.setPixelSize(width()-2);
	meter_label_x=(width()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
  }
}
