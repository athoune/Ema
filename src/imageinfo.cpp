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

#include <QString>
#include <QStringList>

#include "imageinfo.h"

#include "imgutils.h"

// Read metadata
#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/iptc.hpp>

#include <iostream>
#include <iomanip>
#include <cassert>

u8 g_debug_ImageInfo = 0; //EMALOG_DEBUG;

/******************************************************************************/
ImageInfo::ImageInfo() {
	init();
}
ImageInfo::~ImageInfo() {
	purge();
}
void ImageInfo::init() {
	m_originalImage = NULL;

	m_thumbImage = m_scaledImage = m_grayImage = m_HSHistoImage =
	m_HistoImage = m_ColorHistoImage =
	hsvImage = hsvOutImage = h_plane = s_plane = NULL;
	for(int rgb=0; rgb<4; rgb++)
		rgb_plane[rgb] = 0;

	memset(&m_image_info_struct, 0, sizeof(t_image_info_struct ));

	m_sharp32fImage = m_sobelImage = NULL;

	m_sharpnessImage = NULL;
}

void ImageInfo::purge() {
	memset(&m_image_info_struct, 0, sizeof(t_image_info_struct ));
	tmReleaseImage(&m_originalImage);
	tmReleaseImage(&m_HistoImage); // Delete only at the end, because it's always the same size
	tmReleaseImage(&hsvOutImage);
	purgeScaled();
}

QString rational(const QString & str) {
	if(!(str.contains("/"))) return str;

	QString fract = str;
	// split
	QStringList splitted = fract.split('/');
	bool ok;
	int num = splitted[0].toInt(&ok);
	if(!ok) return fract;
	int den = splitted[1].toInt(&ok);
	if(!ok) return fract;

	float val = (float)num / (float)den;

	return fract.sprintf("%g", val);
}

QString rational_1_div(const QString & str) {
	if(!(str.contains("/"))) return str;

	QString fract = str;
	// split
	QStringList splitted = fract.split('/');
	bool ok;
	int num = splitted[0].toInt(&ok);
	if(!ok) return fract;
	int den = splitted[1].toInt(&ok);
	if(!ok) return fract;

	if(num == 0) return fract;
	float val = (float)den / (float)num;

	return fract.sprintf("1/%g", val);
}

float rational_to_float(const QString & str) {
	if(!(str.contains("/"))) {
		//
		float val;
		bool ok;
		val = str.toFloat(&ok);
		if(!ok) return -1.f;
		return val;
	}
	QString fract = str;

	// split
	QStringList splitted = fract.split('/');
	bool ok;
	int num = splitted[0].toInt(&ok);
	if(!ok) return -1.f;
	int den = splitted[1].toInt(&ok);
	if(!ok) return (float)num;

	if(num == 0) {
		if(den == 0) return 0.f; // 0/0 = 0

		return -1.f;
	}
	float val = (float)num / (float)den;

	return val; //fract.sprintf("1/%g", val);
}



void ImageInfo::purgeScaled() {
	// purge info data
	if(m_scaledImage == m_grayImage) {
		m_grayImage = NULL;
	} else {
		tmReleaseImage(&m_grayImage);
	}

	tmReleaseImage(&m_scaledImage);
	tmReleaseImage(&m_thumbImage);
	for(int rgb=0; rgb<4; rgb++)
		tmReleaseImage(&rgb_plane[rgb]);

	tmReleaseImage(&m_sharpnessImage);
	tmReleaseImage(&m_sharp32fImage);
	tmReleaseImage(&m_sobelImage);
	tmReleaseImage(&m_HSHistoImage);
	tmReleaseImage(&m_HistoImage);
	tmReleaseImage(&hsvImage);
	tmReleaseImage(&h_plane);
	tmReleaseImage(&s_plane);
	tmReleaseImage(&m_ColorHistoImage);
}

int ImageInfo::readMetadata(char * filename) {
	// Read metadata
	memset(&m_image_info_struct, 0, sizeof(t_image_info_struct ));

	try {
		Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open( filename );
		assert(image.get() != 0);
		image->readMetadata();

		Exiv2::ExifData &exifData = image->exifData();
		if (exifData.empty()) {
			std::string error(filename);
			error += ": No Exif data found in the file";
			throw Exiv2::Error(1, error);
		}
		Exiv2::ExifData::const_iterator end = exifData.end();

		QString displayStr;

		Exiv2::Exifdatum& exifMaker = exifData["Exif.Image.Make"];
		std::string str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		strncpy(m_image_info_struct.maker, displayStr.ascii(), MAX_EXIF_LEN);

		exifMaker = exifData["Exif.Image.Model"];str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		strncpy(m_image_info_struct.model, displayStr.ascii(), MAX_EXIF_LEN);

		// DateTime
		exifMaker = exifData["Exif.Photo.DateTimeOriginal"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		strncpy(m_image_info_struct.datetime, displayStr.ascii(), MAX_EXIF_LEN);


		// Orientation
		exifMaker = exifData["Exif.Image.Orientation"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		m_image_info_struct.orientation = (char)( displayStr.contains("0") ? 0 : 1);


		// Focal
		exifMaker = exifData["Exif.Photo.FocalLengthIn35mmFilm"];
		str = exifMaker.toString();
		displayStr = QString::fromStdString(str);

		if(QString::compare(displayStr, "0")) {
			exifMaker = exifData["Exif.Photo.FocalLength"]; str = exifMaker.toString();
			displayStr = QString::fromStdString(str);

			bool ok;
			//displayStr = rational(displayStr);
			m_image_info_struct.focal_mm = rational_to_float(displayStr);
			m_image_info_struct.focal_eq135_mm = -1.f;
		} else {
			m_image_info_struct.focal_eq135_mm = rational_to_float(displayStr);
			m_image_info_struct.focal_mm = -1.f;

			displayStr += QString("eq. 35mm");
		}
		if(g_debug_ImageInfo) {
			fprintf(stderr, "Focal : '%s' => %g s %g 35mm\n",
				displayStr.ascii(),
				m_image_info_struct.focal_mm,
				m_image_info_struct.focal_eq135_mm);
		}

		// Aperture
		exifMaker = exifData["Exif.Photo.FNumber"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		m_image_info_struct.aperture = rational_to_float(displayStr);

		// Speed
		exifMaker = exifData["Exif.Photo.ExposureTime"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		m_image_info_struct.speed_s = rational_to_float(displayStr);
		if(g_debug_ImageInfo) {
			fprintf(stderr, "Exposure time : '%s' => %g s\n",
				displayStr.ascii(), m_image_info_struct.speed_s);
		}

		// Speed
		exifMaker = exifData["Exif.Photo.ISOSpeedRatings"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		m_image_info_struct.ISO = (int)rational_to_float(displayStr);

		if(0)
		for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {

/*			str.sprintf("%d ", i->key());
			displayStr += QString(i->key()) ;
*/
			QString str; str = (i->tag());

//			displayStr += str+ QString(" = ") + "\n";
//			str.sprintf("%d ", i->value());

			//displayStr += QString(i->value()) + "\n";

			std::cout << std::setw(44) << std::setfill(' ') << std::left
					<< i->key() << " "
					<< "0x" << std::setw(4) << std::setfill('0') << std::right
					<< std::hex << i->tag() << " "
					<< std::setw(9) << std::setfill(' ') << std::left
					<< i->typeName() << " "
					<< std::dec << std::setw(3)
					<< std::setfill(' ') << std::right
					<< i->count() << "  "
					<< std::dec << i->value()
					<< "\n";
		}


		// IPTC - IPTC - IPTC - IPTC - IPTC - IPTC - IPTC - IPTC - IPTC -
		Exiv2::IptcData &iptcData = image->iptcData();
		if (iptcData.empty()) {
			std::string error(filename);
			error += ": No IPTC data found in the file";
			throw Exiv2::Error(1, error);
		}

		Exiv2::IptcData::iterator endIPTC = iptcData.end();
		for (Exiv2::IptcData::iterator md = iptcData.begin(); md != endIPTC; ++md) {
			std::cout << std::setw(44) << std::setfill(' ') << std::left
					  << md->key() << " "
					  << "0x" << std::setw(4) << std::setfill('0') << std::right
					  << std::hex << md->tag() << " "
					  << std::setw(9) << std::setfill(' ') << std::left
					  << md->typeName() << " "
					  << std::dec << std::setw(3)
					  << std::setfill(' ') << std::right
					  << md->count() << "  "
					  << std::dec << md->value()
					  << std::endl;
		}
		return 0;
	}
	catch (Exiv2::AnyError& e) {
		std::cout << "Caught Exiv2 exception '" << e << "'\n";
		fprintf(stderr, "ImageInfo::%s:%d : ERROR : caught exception => return 0 (no metadata);\n",
				__func__, __LINE__);
		return 0;
	}

	fprintf(stderr, "ImageInfo::%s:%d : ERROR : return 0;\n", __func__, __LINE__);

	return 0;
}
int ImageInfo::loadFile(char * filename) {
	m_image_info_struct.valid = 0;
	tmReleaseImage(&m_originalImage);


	// LOAD IMAGE METADATA
	readMetadata(filename);



	// LOAD IMAGE PIXMAP
	m_originalImage = cvLoadImage(filename);
	if(!m_originalImage) {
		fprintf(stderr, "ImageInfo::%s:%d: cannot load image '%s'\n",
				__func__, __LINE__, filename);
		return -1;
	}
	strcpy(m_image_info_struct.filepath, filename );

	if(g_debug_ImageInfo) {
		fprintf(stderr, "ImageInfo::%s:%d : loaded %dx%d x %d\n", __func__, __LINE__,
			m_originalImage->width, m_originalImage->height,
			m_originalImage->nChannels );
	}
	m_image_info_struct.grayscaled = (m_originalImage->depth == 1);

	m_originalImage = tmAddBorder4x(m_originalImage); // it will purge originalImage
	if(g_debug_ImageInfo) {
		fprintf(stderr, "ImageInfo::%s:%d => pitchedx4: %dx%d x %d\n", __func__, __LINE__,
			m_originalImage->width, m_originalImage->height,
			m_originalImage->nChannels );
	}

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

#define IMGTHUMB_WIDTH	80
#define IMGTHUMB_HEIGHT 80

	// Scale rescaled to thumb size
	if(!m_thumbImage) {
		int th_w = IMGTHUMB_WIDTH;
		int th_h = IMGTHUMB_HEIGHT;
		float th_factor_w = (float)m_originalImage->width / (float)IMGTHUMB_WIDTH;
		float th_factor_h = (float)m_originalImage->height / (float)IMGTHUMB_HEIGHT;
		if(th_factor_w > th_factor_h) {
			// limit w
			th_w = m_originalImage->width * IMGTHUMB_HEIGHT/ m_originalImage->height;
		} else {
			th_h = m_originalImage->height * IMGTHUMB_WIDTH/ m_originalImage->width;
		}

		while((th_w % 4) != 0) { th_w++; }
		while((th_h % 4) != 0) { th_h++; }
		m_thumbImage = tmCreateImage( cvSize(th_w, th_h), IPL_DEPTH_8U, m_originalImage->nChannels);
	}

	cvResize(m_scaledImage, m_thumbImage);
	m_image_info_struct.thumbImage = m_thumbImage;

	if(g_debug_ImageInfo) {
		fprintf(stderr, "ImageInfo::%s:%d : scaled to %dx%d\n", __func__, __LINE__,
			m_scaledImage->width, m_scaledImage->height);
		fprintf(stderr, "ImageInfo::%s:%d : thumb %dx%d\n", __func__, __LINE__,
			m_thumbImage->width, m_thumbImage->height);

		fprintf(stderr, "\nImageInfo::%s:%d : processHSV(m_scaledImage=%dx%d)\n", __func__, __LINE__,
			m_scaledImage->width, m_scaledImage->height);fflush(stderr);
	}
	// process RGB histogram
	processRGB();

	// process color analysis
	processHSV();

	// then sharpness
	if(g_debug_ImageInfo) {
		fprintf(stderr, "ImageInfo::%s:%d : processSharpness(gray=%dx%d)\n", __func__, __LINE__,
			m_grayImage->width, m_grayImage->height);fflush(stderr);
	}
	processSharpness();

	if(g_debug_ImageInfo) {
		fprintf(stderr, "ImageInfo::%s:%d : process done (gray=%dx%d)\n", __func__, __LINE__,
			m_grayImage->width, m_grayImage->height);fflush(stderr);
	}

	/* Compute the final score
		Score is the combination of several criteria :
		- sharpness : proportioanl, and best if superior to 50 %
	*/
	float sharpness_score = tmmin(1.f,
								  2.f * m_image_info_struct.sharpness_score / 100.f);
	float histo_score = tmmin(1.f,
								  2.f * m_image_info_struct.histo_score / 100.f);

	m_image_info_struct.score = sharpness_score
								* histo_score
								* 100.f ; // in percent finally

	// Activate the validation flag
	m_image_info_struct.valid = 1;
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

#define HSVHISTO_WIDTH	90
#define HSVHISTO_HEIGHT	64

	float h_scale = (float)HSVHISTO_WIDTH / (float)H_MAX;
	float s_scale = (float)HSVHISTO_HEIGHT / (float)S_MAX;

	if(!m_HSHistoImage) {
		m_HSHistoImage = tmCreateImage(cvSize(HSVHISTO_WIDTH , HSVHISTO_HEIGHT), IPL_DEPTH_8U, 1);
	} else {
		cvZero(m_HSHistoImage);
	}

	// Then process histogram
	for(int r=0; r<hsvImage->height; r++) {
		u8 * hline = (u8 *)(h_plane->imageData
										+ r * h_plane->widthStep);
		u8 * sline = (u8 *)(s_plane->imageData
										+ r * s_plane->widthStep);
		for(int c = 0; c<s_plane->width; c++) {
			int h = (int)(hline[c]);
			int s = (int)(sline[c]);

			// Accumulate in scaled image for saving time
			int histo_h = (int)roundf(h * h_scale);
			int histo_s = (int)roundf(s * s_scale);
			if(histo_h<m_HSHistoImage->widthStep
			   && histo_s<m_HSHistoImage->height) {
				// Increase image
				u8 * pHisto = // H as columns, S as line
					(u8 *)(m_HSHistoImage->imageData + histo_s * m_HSHistoImage->widthStep)
					+ histo_h;
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

	if(!hsvOutImage) {
		hsvOutImage = tmCreateImage(
			cvSize(m_HSHistoImage->width, m_HSHistoImage->height),
			IPL_DEPTH_8U,
			3);
	}

	// Fill with H,S and use Value for highlighting colors
	for(int r=0; r<hsvOutImage->height; r++) {
		u8 * outline = (u8 *)(hsvOutImage->imageData
						+ r * hsvOutImage->widthStep);
		u8 * histoline = (u8 *)(m_HSHistoImage->imageData
						+ r * m_HSHistoImage->widthStep);
		int c1 = 0;
		for(int c = 0; c1<HSVHISTO_WIDTH;
			c1++, c+=hsvOutImage->nChannels) {
			outline[c] = roundf(c1 / h_scale); // H
			outline[c+1] = roundf(r / h_scale); // S
			outline[c+2] = 64 + (int)tmmin( (float)histoline[c1], 170.f);
					//histoline[c1]>1 ? 255 : 64; //histoline[c1];
		}
	}

	if(!m_ColorHistoImage) {
		m_ColorHistoImage = tmCreateImage(
			cvSize(hsvOutImage->width, hsvOutImage->height),
			IPL_DEPTH_8U,
			3);
	} // else cvZero(m_ColorHistoImage); // don't need to clear, because always the same size

	cvCvtColor(hsvOutImage, m_ColorHistoImage, CV_HSV2BGR);

	m_image_info_struct.hsvImage = m_ColorHistoImage;

	//tmSaveImage(TMP_DIRECTORY "HSHistoColored.ppm", m_ColorHistoImage);
	if(g_debug_ImageInfo) {
		tmSaveImage(TMP_DIRECTORY "HSHistoHSV.ppm", hsvOutImage);
	}

	return 0;
}


int ImageInfo::processRGB() {

	// Compute RGB histogram
	for(int rgb=0; rgb<m_scaledImage->nChannels; rgb++)  {
		rgb_plane[rgb] = tmCreateImage(cvGetSize(m_scaledImage),
									   IPL_DEPTH_8U, 1);
	}

	if(m_scaledImage->nChannels >= 3)
		cvCvtPixToPlane( m_scaledImage, rgb_plane[0], rgb_plane[1], rgb_plane[2], 0 );
	else
		cvCopy(	m_scaledImage, rgb_plane[0]);


	u32 histo_score = 0;
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

		// Store in info structure
		for(int h=0; h<256; h++) {
			if(grayHisto[rgb][h]>hmax) {
				hmax = grayHisto[rgb][h] ; }
			if(grayHisto[rgb][h]>0)
				m_image_info_struct.log_histogram[rgb][h] = log(grayHisto[rgb][h]);
			else
				m_image_info_struct.log_histogram[rgb][h] = 0.f;
		}

		// compute the score for histogram
		float log_hmax = log((float)hmax) / 8.f;
		for(int h=0; h<256; h++) {
			if(grayHisto[rgb][h]>0) {
				if(log(grayHisto[rgb][h])>log_hmax) {
					histo_score++;
				}
			}
		}

	}

	m_image_info_struct.histo_score = (float)histo_score /
									  (float)m_scaledImage->nChannels
									  * 100.f / 256.f;
	return 0;
}


int ImageInfo::processSharpness() {

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

	if(m_sharpnessImage
	   && (m_sharpnessImage->width != scaledSize.width
		   || m_sharpnessImage->height != scaledSize.height) ) {
		tmReleaseImage(&m_sharpnessImage);
	}

	if(!m_sharpnessImage) {
		m_sharpnessImage = tmCreateImage( scaledSize, IPL_DEPTH_8U, 1 );
	} else {
		cvZero(m_sharpnessImage);
	}


	if(m_sobelImage
	   && (m_sobelImage->width != m_grayImage->width
		   || m_sobelImage->height != m_grayImage->height) ) {
		tmReleaseImage(&m_sobelImage);
	}

	if(!m_sobelImage) {
		m_sobelImage = tmCreateImage( size, IPL_DEPTH_16S, 1 );
	} else {
		// no need, because recomputed : cvZero(m_sobelImage);
	}

	if(!m_sobelImage) {
		fprintf(stderr, "ImageInfo::%s:%d : cannot create sobelImage ( size=%dx%d => scaledSize = %dx%d => scale factor=%dx%d)\n",
				__func__, __LINE__,
				size.width, size.height,
				scaledSize.width, scaledSize.height,
				scale_x, scale_y); fflush(stderr);
		return -1;
	}

	if(m_sharp32fImage
	   && (m_sharp32fImage->width != scaledSize.width
		   || m_sharp32fImage->height != scaledSize.height) ) {
		tmReleaseImage(&m_sharp32fImage);
	}

	if(!m_sharp32fImage) {
		m_sharp32fImage = tmCreateImage( scaledSize, IPL_DEPTH_32F, 1 );
	} else {
		cvZero(m_sharp32fImage);
	}

	if(!m_sharp32fImage) {
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

		cvSobel(m_grayImage, m_sobelImage, pass, 1-pass, 3);

		for(int r=0; r< m_sobelImage->height; r++) {
			short * sobelline = (short *)(m_sobelImage->imageData +
									r * m_sobelImage->widthStep);
			int sc_r = r / scale_y;
			if(sc_r>= m_sharp32fImage->height)
				sc_r = m_sharp32fImage->height - 1;

			float * sharpline32f = (float *)(m_sharp32fImage->imageData +
									sc_r * m_sharp32fImage->widthStep);

			for(int c=0; c<m_sobelImage->width; c++) {
				if(abs(sobelline[c])>80) {
					int sc_c = c/scale_x;

					if(sc_c<m_sharp32fImage->width) {
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
	m_image_info_struct.sharpness_score = 0.f;
	for(int sc_r=0; sc_r< m_sharp32fImage->height; sc_r++) {
		float * sharpline32f = (float *)(m_sharp32fImage->imageData +
								sc_r * m_sharp32fImage->widthStep);
		u8 * sharpline = (u8 *)(m_sharpnessImage->imageData +
								sc_r * m_sharpnessImage->widthStep);
		for(int sc_c=0; sc_c<m_sharp32fImage->width; sc_c++) {
			float val = scale * sharpline32f[sc_c];
			if(val >= 255.f) val = 255.f;
			m_image_info_struct.sharpness_score += sharpline32f[sc_c];
			sharpline[sc_c] = (u8)( val );
		}
	}

	m_image_info_struct.sharpness_score = m_image_info_struct.sharpness_score * 100.f/255.f
				  / (float)(m_sharpnessImage->width * m_sharpnessImage->height);
	m_image_info_struct.sharpnessImage = m_sharpnessImage;

	// Process region growing
/*	void tmGrowRegion(unsigned char * growIn, unsigned char * growOut,
		int swidth, int sheight,
		int c, int r,
		unsigned char threshold,
		unsigned char fillValue,
		CvConnectedComp * areaOut);
*/
	IplImage * growImage = tmCreateImage(cvSize(m_sharpnessImage->width,m_sharpnessImage->height),
										 IPL_DEPTH_8U, 1);
	// clear is done
	int r = 0, c=0;
	CvConnectedComp areaOut;
	tmGrowRegion(
			(u8 *)m_sharpnessImage->imageData, //unsigned char * growIn,
			(u8 *)growImage->imageData, //unsigned char * growOut,
			m_sharpnessImage->widthStep, m_sharpnessImage->height,
			c, r,
			180, //unsigned char threshold,
			255, //unsigned char fillValue,
			&areaOut);


	if(g_debug_ImageInfo) {
		tmSaveImage(TMP_DIRECTORY "growImage.pgm", growImage);
		tmSaveImage(TMP_DIRECTORY "sharpImage.pgm", m_sharpnessImage);
	}
	//
	tmReleaseImage(&growImage);

	return 0;
}



