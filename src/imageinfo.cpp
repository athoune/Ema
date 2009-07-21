/***************************************************************************
 *  imageinfo - Information about picture with image processing
 *
 *  Jul 2 21:10:56 2009
 *  Copyright  2007  Christophe Seyve
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "imageinfo.h"

#include "imgutils.h"



u8 g_debug_ImageInfo = 0;

/******************************************************************************/
ImageInfo::ImageInfo() {
	init();
}
ImageInfo::~ImageInfo() {
	purge();
}
void ImageInfo::init() {
	m_originalImage = NULL;

	m_scaledImage = m_grayImage = m_HSHistoImage =
	m_HistoImage = m_ColorHistoImage = hsvImage = h_plane = s_plane = NULL;
	for(int rgb=0; rgb<4; rgb++)
		rgb_plane[rgb] = 0;

	m_sharpnessImage = NULL;
}

void ImageInfo::purge() {
	tmReleaseImage(&m_originalImage);
	tmReleaseImage(&m_HistoImage); // Delete only at the end, because it's always the same size
	purgeScaled();
}

void ImageInfo::purgeScaled() {
	if(m_scaledImage == m_grayImage) {
		m_grayImage = NULL;
	} else {
		tmReleaseImage(&m_grayImage);
	}

	tmReleaseImage(&m_scaledImage);
	for(int rgb=0; rgb<4; rgb++)
		tmReleaseImage(&rgb_plane[rgb]);

	tmReleaseImage(&m_sharpnessImage);
	tmReleaseImage(&m_HSHistoImage);
	tmReleaseImage(&m_HistoImage);
	tmReleaseImage(&hsvImage);
	tmReleaseImage(&h_plane);
	tmReleaseImage(&s_plane);
	tmReleaseImage(&m_ColorHistoImage);
}


int ImageInfo::loadFile(char * filename) {
	tmReleaseImage(&m_originalImage);

	m_originalImage = cvLoadImage(filename);
	if(!m_originalImage) {
		fprintf(stderr, "ImageInfo::%s:%d: cannot load image '%s'\n",
				__func__, __LINE__, filename);
		return -1;
	}

	fprintf(stderr, "ImageInfo::%s:%d : loaded %dx%d x %d\n", __func__, __LINE__,
			m_originalImage->width, m_originalImage->height,
			m_originalImage->nChannels );

	m_originalImage = tmAddBorder4x(m_originalImage); // it will purge originalImage
	fprintf(stderr, "ImageInfo::%s:%d => pitchedx4: %dx%d x %d\n", __func__, __LINE__,
			m_originalImage->width, m_originalImage->height,
			m_originalImage->nChannels );


#define IMGINFO_WIDTH	400
#define IMGINFO_HEIGHT	400

	// Scale to analysis size
//		cvResize(tmpDisplayImage, new_displayImage, CV_INTER_LINEAR );
	int sc_w = IMGINFO_WIDTH;
	int sc_h = IMGINFO_HEIGHT;
	float scale_factor_w = (float)m_originalImage->width / (float)IMGINFO_WIDTH;
	float scale_factor_h = (float)m_originalImage->height / (float)IMGINFO_HEIGHT;
	if(scale_factor_w > scale_factor_h) {
		// limit w
		sc_w = m_originalImage->width * IMGINFO_HEIGHT/m_originalImage->height;
	} else {
		sc_h = m_originalImage->height * IMGINFO_WIDTH/m_originalImage->width;
	}

	while((sc_w % 4) != 0) { sc_w++; }
	while((sc_h % 4) != 0) { sc_h++; }

	if(m_scaledImage
	   && (m_scaledImage->width != sc_w || m_scaledImage->height != sc_h)) {
		// purge scaled images
		purgeScaled();
	}

	// Scale original image to smaller image to fasten later processinggs
	if(!m_scaledImage) {
		m_scaledImage = tmCreateImage(cvSize(sc_w, sc_h),
								  IPL_DEPTH_8U, m_originalImage->nChannels);
	}

	cvResize(m_originalImage, m_scaledImage);

	fprintf(stderr, "ImageInfo::%s:%d : scaled to %dx%d\n", __func__, __LINE__,
			m_scaledImage->width, m_scaledImage->height);

	fprintf(stderr, "\nImageInfo::%s:%d : processHSV(m_scaledImage=%dx%d)\n", __func__, __LINE__,
			m_scaledImage->width, m_scaledImage->height);fflush(stderr);

	// process RGB histogram
	processRGB();

	// process color analysis
	processHSV();

	// then sharpness
	fprintf(stderr, "ImageInfo::%s:%d : processSharpness(gray=%dx%d)\n", __func__, __LINE__,
			m_grayImage->width, m_grayImage->height);fflush(stderr);
	processSharpness();

	fprintf(stderr, "ImageInfo::%s:%d : process done (gray=%dx%d)\n", __func__, __LINE__,
			m_grayImage->width, m_grayImage->height);fflush(stderr);

	return 0;
}

int ImageInfo::processHSV() {
	// Change to HSV
	if(m_scaledImage->nChannels < 3) {
		// Clear histogram and return
		fprintf(stderr, "ImageInfo::%s:%d : not coloured image : nChannels=%d\n", __func__, __LINE__,
			m_scaledImage->nChannels );
		m_grayImage = m_scaledImage;
		return 0;
	}
/*
void cvCvtColor( const CvArr* src, CvArr* dst, int code );

RGB<=>HSV (CV_BGR2HSV, CV_RGB2HSV, CV_HSV2BGR, CV_HSV2RGB)


// In case of 8-bit and 16-bit images
// R, G and B are converted to floating-point format and scaled to fit 0..1 range
The values are then converted to the destination data type:
	8-bit images:
		V <- V*255, S <- S*255, H <- H/2 (to fit to 0..255)

  */


	if(!hsvImage)
		hsvImage = tmCreateImage(
			cvSize(m_scaledImage->width, m_scaledImage->height),
			IPL_DEPTH_8U,
			m_scaledImage->nChannels);
	if(m_scaledImage->nChannels == 3) {
		//cvCvtColor(m_scaledImage, hsvImage, CV_RGB2HSV);
		cvCvtColor(m_scaledImage, hsvImage, CV_BGR2HSV);
	} else {
		cvCvtColor(m_scaledImage, hsvImage, CV_BGR2HSV);
	}

	if(!h_plane) h_plane = tmCreateImage( cvGetSize(hsvImage), IPL_DEPTH_8U, 1 );
	if(!s_plane) s_plane = tmCreateImage( cvGetSize(hsvImage), IPL_DEPTH_8U, 1 );
	if(!m_grayImage) {
		m_grayImage = tmCreateImage( cvGetSize(hsvImage), IPL_DEPTH_8U, 1 );
	}

	cvCvtPixToPlane( hsvImage, h_plane, s_plane, m_grayImage, 0 );

	if(g_debug_ImageInfo) {
		tmSaveImage(TMP_DIRECTORY "HImage.pgm", h_plane);
		tmSaveImage(TMP_DIRECTORY "SImage.pgm", s_plane);
		tmSaveImage(TMP_DIRECTORY "VImage.pgm", m_grayImage);
	}


	// save image for debug
	if(g_debug_ImageInfo) {
		tmSaveImage(TMP_DIRECTORY "HSVimage.ppm", hsvImage);
	}
	if(!m_HSHistoImage) {
		m_HSHistoImage = tmCreateImage(cvSize(H_MAX, S_MAX), IPL_DEPTH_8U, 1);
	} else
		cvZero(m_HSHistoImage);

	// Then process histogram
	for(int r=0; r<hsvImage->height; r++) {
		u8 * hline = (u8 *)(h_plane->imageData
										+ r * h_plane->widthStep);
		u8 * sline = (u8 *)(s_plane->imageData
										+ r * s_plane->widthStep);
		for(int c = 0; c<s_plane->width; c++) {
			int h = (int)(hline[c]);
			int s = (int)(sline[c]);
			if(h<m_HSHistoImage->widthStep
			   && s<m_HSHistoImage->height) {
				// Increase image
				u8 * pHisto = // H as columns, S as line
					(u8 *)(m_HSHistoImage->imageData + s * m_HSHistoImage->widthStep)
					+ h;
				u8 val = *pHisto;
				if(val < 255) {
					*pHisto = val+1;
				}
			}
		}
	}

	// save image for debug
	if(g_debug_ImageInfo) {
		tmSaveImage(TMP_DIRECTORY "HSHisto.pgm", m_HSHistoImage);
	}

	IplImage * hsvOutImage = tmCreateImage(
			cvSize(H_MAX, S_MAX),
			IPL_DEPTH_8U,
			3);
	// Fill with H,S and use Value for highlighting colors
	for(int r=0; r<hsvOutImage->height; r++) {
		u8 * outline = (u8 *)(hsvOutImage->imageData
										+ r * hsvOutImage->widthStep);
		u8 * histoline = (u8 *)(m_HSHistoImage->imageData
										+ r * m_HSHistoImage->widthStep);
		int c1 = 0;
		for(int c = 0; c1<H_MAX;
			c1++, c+=hsvOutImage->nChannels) {
			outline[c] = c1; // H
			outline[c+1] = r; // S
			outline[c+2] = 64 + (int)tmmin( (float)histoline[c1]*2.f, 191.f);
					//histoline[c1]>1 ? 255 : 64; //histoline[c1];
		}
	}
	if(!m_ColorHistoImage) {
		m_ColorHistoImage = tmCreateImage(
			cvSize(H_MAX, S_MAX),
			IPL_DEPTH_8U,
			3);
	} else
		cvZero(m_ColorHistoImage);

	cvCvtColor(hsvOutImage, m_ColorHistoImage, CV_HSV2BGR);
	//tmSaveImage(TMP_DIRECTORY "HSHistoColored.ppm", m_ColorHistoImage);
	if(g_debug_ImageInfo) {
		tmSaveImage(TMP_DIRECTORY "HSHistoHSV.ppm", hsvOutImage);
	}
	tmReleaseImage(&hsvOutImage);

	return 0;
}

int ImageInfo::processRGB() {

	IplImage * histo_plane[3] = {NULL, NULL, NULL};
	// Compute RGB histogram
	for(int rgb=0; rgb<m_scaledImage->nChannels; rgb++)  {
		rgb_plane[rgb] = tmCreateImage(cvGetSize(m_scaledImage),
									   IPL_DEPTH_8U, 1);
		histo_plane[rgb] = tmCreateImage(cvSize(256, 100), IPL_DEPTH_8U, 1);
	}

	if(m_scaledImage->nChannels >= 3)
		cvCvtPixToPlane( m_scaledImage, rgb_plane[0], rgb_plane[1], rgb_plane[2], 0 );
	else
		cvCopy(	m_scaledImage, rgb_plane[0]);

	u32 hmax = 0;
	u32 grayHisto[3][256];
	for(int rgb=0; rgb<m_scaledImage->nChannels; rgb++)  {
		memset(grayHisto[rgb], 0, sizeof(u32)*256);
		for(int r=0; r<rgb_plane[rgb]->height; r++) {
			u8 * grayline = (u8 *)(rgb_plane[rgb]->imageData + r*rgb_plane[rgb]->widthStep);

			for(int c = 0; c<rgb_plane[rgb]->width; c++) {
				grayHisto[rgb][ grayline[c] ]++;
			}
		}
		for(int h=0; h<256; h++) {
			if(grayHisto[rgb][h]>hmax) { hmax = grayHisto[rgb][h] ; }
		}
	}

	// Draw histogram image
	if(!m_HistoImage) {
		m_HistoImage = tmCreateImage(cvSize(256, 100), IPL_DEPTH_8U, m_scaledImage->nChannels);
	} else {
		cvZero(m_HistoImage);
	}
	float divlogmax = 100.f / log((float)hmax) ;
	for(int rgb=0; rgb<m_scaledImage->nChannels; rgb++)  {
		cvZero(histo_plane[rgb]);

		for(int h = 0; h< 256; h++) {
			if(grayHisto[rgb][h]) {
				int val = 100 - log((float)grayHisto[rgb][h]) * divlogmax;
				if(val < 100) {
					cvLine(histo_plane[rgb], cvPoint(h, 100),
						   cvPoint(h, val),
						   cvScalarAll(192), 1);
					cvLine(histo_plane[rgb], cvPoint(h, val),
						   cvPoint(h, val),
						   cvScalarAll(255), 1);
				}
			}
		}
	}
	// Mix R,G,B planes
	if(m_scaledImage->nChannels >= 3)
		cvCvtPlaneToPix( histo_plane[0], histo_plane[1], histo_plane[2], 0, m_HistoImage );
	else
		cvCopy(histo_plane[0], m_HistoImage);

	for(int rgb=0; rgb<m_scaledImage->nChannels; rgb++)  {
		tmReleaseImage(&rgb_plane[rgb]);
		tmReleaseImage(&histo_plane[rgb]);
	}

	return 0;
}

int ImageInfo::processSharpness() {
	tmReleaseImage(&m_sharpnessImage);

	// Process sobel
	CvSize size = cvGetSize(m_grayImage);

	CvSize scaledSize;
	if(size.width > size.height) {
		scaledSize.height = 16;
		scaledSize.width = size.width * scaledSize.height / size.height + 1 ;
	} else {
		scaledSize.width = 16;
		scaledSize.height = size.height * scaledSize.width / size.width + 1 ;
	}
	int scale_x = size.width / scaledSize.width;
	int scale_y = size.height / scaledSize.height;

	if(g_debug_ImageInfo) {
		fprintf(stderr, "ImageInfo::%s:%d : size=%dx%d => scaledSize = %dx%d => scale factor=%dx%d\n",
				__func__, __LINE__,
				size.width, size.height,
				scaledSize.width, scaledSize.height,
				scale_x, scale_y); fflush(stderr);
	}
	m_sharpnessImage = tmCreateImage( scaledSize, IPL_DEPTH_8U, 1 );

	IplImage * sobelImage = tmCreateImage( size, IPL_DEPTH_16S, 1 );
	if(!sobelImage) {
		fprintf(stderr, "ImageInfo::%s:%d : cannot create sobelImage ( size=%dx%d => scaledSize = %dx%d => scale factor=%dx%d)\n",
				__func__, __LINE__,
				size.width, size.height,
				scaledSize.width, scaledSize.height,
				scale_x, scale_y); fflush(stderr);
		return -1;
	}

	IplImage * sharpImage = tmCreateImage( scaledSize, IPL_DEPTH_32F, 1 );
	if(!sharpImage) {
		fprintf(stderr, "ImageInfo::%s:%d : cannot create sharpImage ( size=%dx%d => scaledSize = %dx%d => scale factor=%dx%d)\n",
				__func__, __LINE__,
				size.width, size.height,
				scaledSize.width, scaledSize.height,
				scale_x, scale_y); fflush(stderr);
		return -1;
	}

	float valmax = 1.f;
	for(int pass=0; pass<2; pass++) {
		if(g_debug_ImageInfo) {
			fprintf(stderr, "ImageInfo::%s:%d : cvSobel(m_grayImage, sobelImage, pass=%d)\n",
				__func__, __LINE__, pass); fflush(stderr);
		}

		cvSobel(m_grayImage, sobelImage, pass, 1-pass, 3);


		for(int r=0; r< sobelImage->height; r++) {
			short * sobelline = (short *)(sobelImage->imageData +
									r * sobelImage->widthStep);
			int sc_r = r / scale_y;
			if(sc_r>=sharpImage->height)
				sc_r = sharpImage->height - 1;

			float * sharpline32f = (float *)(sharpImage->imageData +
									sc_r * sharpImage->widthStep);

			for(int c=0; c<sobelImage->width; c++) {
				if(abs(sobelline[c])>80) {
					int sc_c = c/scale_x;

					if(sc_c<sharpImage->width) {
						sharpline32f[ sc_c ] ++;
						if(sharpline32f[ sc_c ] > valmax) valmax = sharpline32f[ sc_c ];
					}
				}
			}
		}
	}

	// Scale image
	//float scale = 255.f / valmax;
	float scale = 1.f; //255.f / 64.f;
	m_sharpness = 0.f;
	for(int sc_r=0; sc_r< sharpImage->height; sc_r++) {
		float * sharpline32f = (float *)(sharpImage->imageData +
								sc_r * sharpImage->widthStep);
		u8 * sharpline = (u8 *)(m_sharpnessImage->imageData +
								sc_r * m_sharpnessImage->widthStep);
		for(int sc_c=0; sc_c<sharpImage->width; sc_c++) {
			float val = scale * sharpline32f[sc_c];
			if(val >= 255.f) val = 255.f;
			m_sharpness += sharpline32f[sc_c];
			sharpline[sc_c] = (u8)( val );
		}
	}

	m_sharpness = m_sharpness * 100.f/255.f
				  / (float)(m_sharpnessImage->width * m_sharpnessImage->height);

	if(g_debug_ImageInfo) {
		tmSaveImage(TMP_DIRECTORY "sharpImage.pgm", m_sharpnessImage);
	}

	if(g_debug_ImageInfo) {
		fprintf(stderr, "ImageInfo::%s:%d : purge sharpImage=%p sobelImage=%p\n",
				__func__, __LINE__, sharpImage, sobelImage); fflush(stderr);
	}
	tmReleaseImage(&sharpImage);
	if(g_debug_ImageInfo) {
		fprintf(stderr, "ImageInfo::%s:%d : purge sharpImage=%p sobelImage=%p\n",
				__func__, __LINE__, sharpImage, sobelImage); fflush(stderr);
	}
	tmReleaseImage(&sobelImage);




	return 0;
}

