/***************************************************************************
 *            imageinfowidget.cpp : display information by image processing
 *
 *  Wed Jun 11 08:47:41 2009
 *  Copyright  2009  Christophe Seyve
 *  Email cseyve@free.fr
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "imageinfowidget.h"
#include "ui_imageinfowidget.h"

#include <cv.h>
#include <cv.hpp>
#include "imgutils.h"


ImageInfoWidget::ImageInfoWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ImageInfoWidget)
{
	m_imgProc = NULL;
	m_ui->setupUi(this);

	m_starButtons[0] = m_ui->scoreButton0;
	m_starButtons[1] = m_ui->scoreButton1;
	m_starButtons[2] = m_ui->scoreButton2;
	m_starButtons[3] = m_ui->scoreButton3;
	m_starButtons[4] = m_ui->scoreButton4;
	m_starButtons[5] = m_ui->scoreButton5;

	m_curInfo = NULL;
}

// Read metadata
#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>


ImageInfoWidget::~ImageInfoWidget()
{
	delete m_ui;

	delete m_imgProc;
}



static u32 * grayToBGR32 = NULL;
static void init_grayToBGR32()
{
	if(grayToBGR32) {
		return;
	}

	grayToBGR32 = new u32 [256];
	for(int c = 0; c<256; c++) {
		int Y = c;
		u32 B = Y;// FIXME
		u32 G = Y;
		u32 R = Y;
		grayToBGR32[c] = (R << 16) | (G<<8) | (B<<0);
	}

}

QImage iplImageToQImage(IplImage * iplImage) {
	if(!iplImage)
		return QImage();


	int depth = iplImage->nChannels;

	bool rgb24_to_bgr32 = false;
	if(depth == 3  ) {// RGB24 is obsolete on Qt => use 32bit instead
		depth = 4;
		rgb24_to_bgr32 = true;
	}

	u32 * grayToBGR32palette = grayToBGR32;
	bool gray_to_bgr32 = false;

	if(depth == 1) {// GRAY is obsolete on Qt => use 32bit instead
		depth = 4;
		gray_to_bgr32 = true;

		init_grayToBGR32();
		grayToBGR32palette = grayToBGR32;
	}

	int orig_width = iplImage->width;

	QImage qImage(orig_width, iplImage->height,
				  8 * depth);
				//  depth > 1 ? QImage::Format_RGB32 : QImage::Format_Indexed8);
	memset(qImage.bits(), 0, orig_width*iplImage->height*depth);

	switch(iplImage->depth) {
	default:
		fprintf(stderr, "imageinfowidget %s:%d : Unsupported depth = %d\n", __func__, __LINE__, iplImage->depth);
		break;

	case IPL_DEPTH_8U: {
		if(!rgb24_to_bgr32 && !gray_to_bgr32) {
			if(iplImage->nChannels != 4) {
				for(int r=0; r<iplImage->height; r++) {
					// NO need to swap R<->B
					memcpy(qImage.bits() + r*orig_width*depth,
						iplImage->imageData + r*iplImage->widthStep,
						orig_width*depth);
				}
			} else {
				for(int r=0; r<iplImage->height; r++) {
					// need to swap R<->B
					u8 * buf_out = (u8 *)(qImage.bits()) + r*orig_width*depth;
					u8 * buf_in = (u8 *)(iplImage->imageData) + r*iplImage->widthStep;
					memcpy(qImage.bits() + r*orig_width*depth,
						iplImage->imageData + r*iplImage->widthStep,
						orig_width*depth);

					for(int pos4 = 0 ; pos4<orig_width*depth; pos4+=depth,
						buf_out+=4, buf_in+=depth
						 ) {
						buf_out[2] = buf_in[0];
						buf_out[0] = buf_in[2];
					}
				}
			}
		}
		else if(rgb24_to_bgr32) {
			// RGB24 to BGR32
			u8 * buffer3 = (u8 *)iplImage->imageData;
			u8 * buffer4 = (u8 *)qImage.bits();
			int orig_width4 = 4 * orig_width;

			for(int r=0; r<iplImage->height; r++)
			{
				int pos3 = r * iplImage->widthStep;
				int pos4 = r * orig_width4;
				for(int c=0; c<orig_width; c++, pos3+=3, pos4+=4)
				{
					//buffer4[pos4 + 2] = buffer3[pos3];
					//buffer4[pos4 + 1] = buffer3[pos3+1];
					//buffer4[pos4    ] = buffer3[pos3+2];
					buffer4[pos4   ] = buffer3[pos3];
					buffer4[pos4 + 1] = buffer3[pos3+1];
					buffer4[pos4 + 2] = buffer3[pos3+2];
				}
			}
		} else if(gray_to_bgr32) {
			for(int r=0; r<iplImage->height; r++)
			{
				u32 * buffer4 = (u32 *)qImage.bits() + r*qImage.width();
				u8 * bufferY = (u8 *)(iplImage->imageData + r*iplImage->widthStep);
				for(int c=0; c<orig_width; c++) {
					buffer4[c] = grayToBGR32palette[ (int)bufferY[c] ];
				}
			}
		}
		}break;
	case IPL_DEPTH_16S: {
		if(!rgb24_to_bgr32) {

			u8 * buffer4 = (u8 *)qImage.bits();
			short valmax = 0;

			for(int r=0; r<iplImage->height; r++)
			{
				short * buffershort = (short *)(iplImage->imageData + r*iplImage->widthStep);
				for(int c=0; c<iplImage->width; c++)
					if(buffershort[c]>valmax)
						valmax = buffershort[c];
			}

			if(valmax>0)
				for(int r=0; r<iplImage->height; r++)
				{
					short * buffer3 = (short *)(iplImage->imageData
									+ r * iplImage->widthStep);
					int pos3 = 0;
					int pos4 = r * orig_width;
					for(int c=0; c<orig_width; c++, pos3++, pos4++)
					{
						int val = abs((int)buffer3[pos3]) * 255 / valmax;
						if(val > 255) val = 255;
						buffer4[pos4] = (u8)val;
					}
				}
		}
		else {
			u8 * buffer4 = (u8 *)qImage.bits();
			if(depth == 3) {

				for(int r=0; r<iplImage->height; r++)
				{
					short * buffer3 = (short *)(iplImage->imageData + r * iplImage->widthStep);
					int pos3 = 0;
					int pos4 = r * orig_width*4;
					for(int c=0; c<orig_width; c++, pos3+=3, pos4+=4)
					{
						buffer4[pos4   ] = buffer3[pos3];
						buffer4[pos4 + 1] = buffer3[pos3+1];
						buffer4[pos4 + 2] = buffer3[pos3+2];
					}
				}
			} else if(depth == 1) {
				short valmax = 0;
				short * buffershort = (short *)(iplImage->imageData);
				for(int pos=0; pos< iplImage->widthStep*iplImage->height; pos++)
					if(buffershort[pos]>valmax)
						valmax = buffershort[pos];

				if(valmax>0) {
					for(int r=0; r<iplImage->height; r++)
					{
						short * buffer3 = (short *)(iplImage->imageData
											+ r * iplImage->widthStep);
						int pos3 = 0;
						int pos4 = r * orig_width;
						for(int c=0; c<orig_width; c++, pos3++, pos4++)
						{
							int val = abs((int)buffer3[pos3]) * 255 / valmax;
							if(val > 255) val = 255;
							buffer4[pos4] = (u8)val;
						}
					}
				}
			}
		}
		}break;
	case IPL_DEPTH_16U: {
		if(!rgb24_to_bgr32) {

			unsigned short valmax = 0;

			for(int r=0; r<iplImage->height; r++)
			{
				unsigned short * buffershort = (unsigned short *)(iplImage->imageData + r*iplImage->widthStep);
				for(int c=0; c<iplImage->width; c++)
					if(buffershort[c]>valmax)
						valmax = buffershort[c];
			}

			if(valmax>0) {
				if(!gray_to_bgr32) {
					u8 * buffer4 = (u8 *)qImage.bits();
					for(int r=0; r<iplImage->height; r++)
					{
						unsigned short * buffer3 = (unsigned short *)(iplImage->imageData
										+ r * iplImage->widthStep);
						int pos3 = 0;
						int pos4 = r * orig_width;
						for(int c=0; c<orig_width; c++, pos3++, pos4++)
						{
							int val = abs((int)buffer3[pos3]) * 255 / valmax;
							if(val > 255) val = 255;
							buffer4[pos4] = (u8)val;
						}
					}
				} else {
					u32 * buffer4 = (u32 *)qImage.bits();
					for(int r=0; r<iplImage->height; r++)
					{
						unsigned short * buffer3 = (unsigned short *)(iplImage->imageData
										+ r * iplImage->widthStep);
						int pos3 = 0;
						int pos4 = r * orig_width;
						for(int c=0; c<orig_width; c++, pos3++, pos4++)
						{
							int val = abs((int)buffer3[pos3]) * 255 / valmax;
							if(val > 255) val = 255;
							buffer4[pos4] = grayToBGR32palette[ val ];
						}
					}
				}
			}
		}
		else {
			fprintf(stderr, "imageinfowidget %s:%d : U16  depth = %d -> BGR32\n", __func__, __LINE__, iplImage->depth);
			u8 * buffer4 = (u8 *)qImage.bits();
			if(depth == 3) {

				for(int r=0; r<iplImage->height; r++)
				{
					short * buffer3 = (short *)(iplImage->imageData + r * iplImage->widthStep);
					int pos3 = 0;
					int pos4 = r * orig_width*4;
					for(int c=0; c<orig_width; c++, pos3+=3, pos4+=4)
					{
						buffer4[pos4   ] = buffer3[pos3]/256;
						buffer4[pos4 + 1] = buffer3[pos3+1]/256;
						buffer4[pos4 + 2] = buffer3[pos3+2]/256;
					}
				}
			} else if(depth == 1) {
				short valmax = 0;
				short * buffershort = (short *)(iplImage->imageData);
				for(int pos=0; pos< iplImage->widthStep*iplImage->height; pos++)
					if(buffershort[pos]>valmax)
						valmax = buffershort[pos];

				if(valmax>0)
					for(int r=0; r<iplImage->height; r++)
					{
						short * buffer3 = (short *)(iplImage->imageData
											+ r * iplImage->widthStep);
						int pos3 = 0;
						int pos4 = r * orig_width;
						for(int c=0; c<orig_width; c++, pos3++, pos4++)
						{
							int val = abs((int)buffer3[pos3]) * 255 / valmax;
							if(val > 255) val = 255;
							buffer4[pos4] = (u8)val;
						}
					}
			}
		}
		}break;
	}

	if(qImage.depth() == 8) {
		qImage.setNumColors(256);

		for(int c=0; c<256; c++) {
			qImage.setColor(c, qRgb(c,c,c));
		}
	}
	return qImage;
}

/*
t_image_info_struct ImageInfoWidget::getImageInfo() {
	t_image_info_struct l_info;
	memset(&l_info, 0, sizeof(t_image_info_struct));
	if(m_imgProc) {
		l_info = m_imgProc->getImageInfo();
	}
	return l_info;
}*/


/* Set the background image */
void ImageInfoWidget::setImageFile(const QString &  imagePath) {
	// Load image on image processing object
	if(!m_imgProc) {
		m_imgProc = new ImageInfo();
	}

	QByteArray arrStr = imagePath.toUtf8();
	m_imgProc->loadFile( arrStr.data() );

	// RGB Histogram
	IplImage * histo = m_imgProc->getHistogram();
	if(histo) {
		// Display in label
		QImage img = iplImageToQImage(histo).scaled(
				m_ui->sharpnessImageLabel->width(),
				m_ui->sharpnessImageLabel->height(),
				Qt::KeepAspectRatio
				);
		m_ui->histoImageLabel->setPixmap(QPixmap::fromImage(img));
	}


	// And sharpness
	IplImage * sharpMask = m_imgProc->getSharpnessImage();
	if(sharpMask) {
		// Display in label
		QImage img = iplImageToQImage(sharpMask).scaled(
				m_ui->sharpnessImageLabel->width(),
				m_ui->sharpnessImageLabel->height(),
				Qt::KeepAspectRatio
				);
		m_ui->sharpnessImageLabel->setPixmap(QPixmap::fromImage(img));
	}

	// Then process HSV histogram
	IplImage * colHisto = m_imgProc->getColorHistogram();
	if(colHisto) {
		// Display in label
		QImage img = iplImageToQImage(colHisto).scaled(
				m_ui->hsvImageLabel->width(),
				m_ui->hsvImageLabel->height(),
				Qt::KeepAspectRatio
				);
		m_ui->hsvImageLabel->setPixmap(QPixmap::fromImage(img));
	}
}


void ImageInfoWidget::setImageInfo(t_image_info_struct * pinfo) {
	if(!pinfo
	   ) {
		// FIXME : clear display

		return ;
	}
	if(pinfo == m_curInfo) {
		// same
		return;
	}
	m_curInfo = pinfo;
	// try tu use sharpness
	int level = roundf(pinfo->score * 5.f / 100.f);
	if(level > 5) { level = 5; }

	if(level > 0) {// don't load picture if not necessary
		QPixmap starOn(":/icons/icons/star_on.png");
		for(int star = 1; star <= level; star++) {
			m_starButtons[star]->setPixmap(starOn);
		}
	}

	if(level < 5) {
		QPixmap starOff(":/icons/icons/star_off.png");
		for(int star = level+1; star < 6; star++) {
			m_starButtons[star]->setPixmap(starOff);
		}
	}

	// RGB Histogram
	IplImage * histo =
			drawHistogram(pinfo->log_histogram, pinfo->grayscaled);

	if(histo) {
		// Display in label
		QImage img = iplImageToQImage(histo).scaled(
				m_ui->sharpnessImageLabel->width(),
				m_ui->sharpnessImageLabel->height(),
				Qt::KeepAspectRatio
				);
		int sh = tmmin(100, (int)pinfo->histo_score);
		m_ui->histoDial->setValue(sh);
		m_ui->histoImageLabel->setPixmap(QPixmap::fromImage(img));
	}

	// And sharpness
	IplImage * sharpMask = pinfo->sharpnessImage;
	if(sharpMask) {
		// Display in label
		QImage img = iplImageToQImage(sharpMask).scaled(
				m_ui->sharpnessImageLabel->width(),
				m_ui->sharpnessImageLabel->height(),
				Qt::KeepAspectRatio
				);
		m_ui->sharpnessImageLabel->setPixmap(QPixmap::fromImage(img));
		int sh = tmmin(100, (int)pinfo->sharpness_score);
		m_ui->sharpnessDial->setValue(sh);
	}

	// Then process HSV histogram
	IplImage * colHisto = pinfo->hsvImage;
	if(colHisto) {
		// Display in label
		QImage img = iplImageToQImage(colHisto).scaled(
				m_ui->hsvImageLabel->width(),
				m_ui->hsvImageLabel->height(),
				Qt::KeepAspectRatio
				);
		m_ui->hsvImageLabel->setPixmap(QPixmap::fromImage(img));
	}
}


void ImageInfoWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

/* Manually force score of a picture */
void ImageInfoWidget::forceScore(int percent) {
	if(!m_curInfo) return;

	m_curInfo->score = percent;
	t_image_info_struct * pinfo = m_curInfo;
	m_curInfo = 0; // to force update
	setImageInfo(pinfo);
}


void ImageInfoWidget::on_scoreButton0_clicked() {
	forceScore(0);
}
void ImageInfoWidget::on_scoreButton1_clicked() {
	forceScore(20);
}

void ImageInfoWidget::on_scoreButton2_clicked() {
	forceScore(40);
}
void ImageInfoWidget::on_scoreButton3_clicked() {
	forceScore(60);
}
void ImageInfoWidget::on_scoreButton4_clicked() {
	forceScore(80);
}

void ImageInfoWidget::on_scoreButton5_clicked() {
	forceScore(100);
}













