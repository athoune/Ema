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
}

ImageInfoWidget::~ImageInfoWidget()
{
	delete m_ui;

	delete m_imgProc;
}



static u32 * grayToBGR32 = NULL;
static u32 * grayToBGR32False = NULL;
static u32 * grayToBGR32Red = NULL;

static void init_grayToBGR32()
{
	if(grayToBGR32) {
		return;
	}

	grayToBGR32 = new u32 [256];
	grayToBGR32False = new u32 [256];
	grayToBGR32Red = new u32 [256];
	for(int c = 0; c<256; c++) {
		int Y = c;
		u32 B = Y;// FIXME
		u32 G = Y;
		u32 R = Y;
		grayToBGR32[c] = grayToBGR32Red[c] =
			grayToBGR32False[c] = (R << 16) | (G<<8) | (B<<0);
	}

	// Add false colors
	grayToBGR32[COLORMARK_CORRECTED] = // GREEN
		grayToBGR32False[COLORMARK_CORRECTED] = (255 << 8);
		//mainImage.setColor(COLORMARK_CORRECTED, qRgb(0,255,0));
	// YELLOW
	grayToBGR32False[COLORMARK_REFUSED] = (255 << 8) | (255 << 16);
				//mainImage.setColor(COLORMARK_REFUSED, qRgb(255,255,0));
	grayToBGR32False[COLORMARK_FAILED] =
			grayToBGR32Red[COLORMARK_FAILED] = (255 << 16);
				//mainImage.setColor(COLORMARK_FAILED, qRgb(255,0,0));
	grayToBGR32False[COLORMARK_CURRENT] = // BLUE
	//		grayToBGR32Red[COLORMARK_CURRENT] =
										(255 << 0);
				//mainImage.setColor(COLORMARK_CURRENT, qRgb(0,0,255));
}

QImage iplImageToQImage(IplImage * iplImage, bool false_colors, bool red_only ) {
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
		if(!false_colors)
			grayToBGR32palette = grayToBGR32;
		else if(red_only)
			grayToBGR32palette = grayToBGR32Red;
		else
			grayToBGR32palette = grayToBGR32False;
	}

	int orig_width = iplImage->width;
	if((orig_width % 2) == 1)
		orig_width--;

	QImage qImage(orig_width, iplImage->height, depth>0 ? QImage::Format_RGB32 : QImage::Format_Indexed8);
	memset(qImage.bits(), 0, orig_width*iplImage->height*depth);

	/*
	if(iplImage->nChannels == 4)
	{
		fprintf(stderr, "[TamanoirApp]::%s:%d : ORIGINAL DEPT IS RGBA depth = %d\n"
				"\trgb24_to_bgr32=%c gray_to_bgr32=%c\n",
				__func__, __LINE__, iplImage->nChannels,
				rgb24_to_bgr32? 'T':'F',
				gray_to_bgr32 ? 'T':'F'
				);
	}*/

	switch(iplImage->depth) {
	default:
		fprintf(stderr, "[TamanoirApp]::%s:%d : Unsupported depth = %d\n", __func__, __LINE__, iplImage->depth);
		break;

	case IPL_DEPTH_8U: {
		if(!rgb24_to_bgr32 && !gray_to_bgr32) {
			if(iplImage->nChannels != 4) {
				for(int r=0; r<iplImage->height; r++) {
					// NO need to swap R<->B
					memcpy(qImage.bits() + r*orig_width*depth,
						iplImage->imageData + r*iplImage->widthStep, orig_width*depth);
				}
			} else {
				for(int r=0; r<iplImage->height; r++) {
					// need to swap R<->B
					u8 * buf_out = (u8 *)(qImage.bits()) + r*orig_width*depth;
					u8 * buf_in = (u8 *)(iplImage->imageData) + r*iplImage->widthStep;
					memcpy(qImage.bits() + r*orig_width*depth,
						iplImage->imageData + r*iplImage->widthStep, orig_width*depth);

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
				for(int c=0; c<iplImage->width; c++) {
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
			fprintf(stderr, "[TamanoirApp]::%s:%d : U16  depth = %d -> BGR32\n", __func__, __LINE__, iplImage->depth);
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
			/* False colors
			int R=c, G=c, B=c;
			if(c<128) B = (255-2*c); else B = 0;
			if(c>128) R = 2*(c-128); else R = 0;
			G = abs(128-c)*2;

			qImage.setColor(c, qRgb(R,G,B));
			*/
			qImage.setColor(c, qRgb(c,c,c));
		}
	}
	return qImage;
}


/* Set the background image */
void ImageInfoWidget::setImageFile(const QString &  imagePath) {
	// Load image on image processing object
	if(!m_imgProc) {
		m_imgProc = new ImageInfo();
	}

	QByteArray arrStr = imagePath.toUtf8();
	m_imgProc->loadFile( arrStr.data() );

	// Then process histogram
	IplImage * colHisto = m_imgProc->getColorHistogram();
	if(colHisto) {
		// Display in label
		QImage img = iplImageToQImage(colHisto, false, false);
		m_ui->globalImageLabel->setPixmap(QPixmap::fromImage(img));
	}

	// And sharpness
	IplImage * sharpMask = m_imgProc->getSharpnessImage();
	if(sharpMask) {
		// Display in label
		QImage img = iplImageToQImage(sharpMask, false, false).scaled(
				m_ui->sharpnessImageLabel->width(),
				m_ui->sharpnessImageLabel->height(),

				Qt::KeepAspectRatio
				);
		m_ui->sharpnessImageLabel->setPixmap(QPixmap::fromImage(img));
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
















