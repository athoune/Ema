/***************************************************************************
 *  imageinfo - Information about picture with image processing
 *
 *  Jul 2 21:10:56 2009
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef IMAGEINFO_H
#define IMAGEINFO_H
#include <cv.h>
#include <cv.hpp>
#include <highgui.h>

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN	512
#endif

#define MAX_EXIF_LEN	32

// Debug modes
#define EMALOG_TRACE	5
#define EMALOG_DEBUG	4
#define EMALOG_INFO		-1
#define EMALOG_WARNING	-2
#define EMALOG_ERROR	-3

#include <QImage>


/** @breif Convert an OpenCV IplIMage to a QImage */
QImage iplImageToQImage(IplImage * iplImage);

/** @brief Useful information for sorting pictures*/
typedef struct {
	char filepath[MAX_PATH_LEN];	/*! Full path of image file */

	unsigned char valid;		/*! Valid info flag */

	// EXIF TAGS
	char maker[MAX_EXIF_LEN];	/*! Company which produces this camera */
	char model[MAX_EXIF_LEN];	/*! Model of this camera */

	char datetime[MAX_EXIF_LEN];	/*! Date/time of the shot */


	char orientation;			/*! Image orientation : 0 for horizontal (landscape), 1 for vertical (portrait) */
	float focal_mm;				/*! Real focal in mm */
	float focal_eq135_mm;		/*! 135mm equivalent focal in mm (if available) */
	float aperture;				/*! F Number */
	float speed_s;				/*! Speed = shutter opening time in seconds */
	int ISO;					/*! ISO Sensitivity */

	// IPTC TAGS
// Ref: /usr/share/doc/libexiv2-doc/html/tags-iptc.html
//0x005a 	90 	Iptc.Application2.City 	String 	No 	No 	0 	32 	Identifies city of object data origin according to guidelines established by the provider.
	char iptc_city[MAX_EXIF_LEN];		/*! IPTC City name, field Iptc.Application2.City */
//0x005c 	92 	Iptc.Application2.SubLocation 	String 	No 	No 	0 	32 	Identifies the location within a city from which the object data originates
	char iptc_provincestate[MAX_EXIF_LEN];		/*! IPTC Province/State name, field Iptc.Application2.Provincestate */
//0x005f 	95 	Iptc.Application2.ProvinceState 	String 	No 	No 	0 	32 	Identifies Province/State of origin according to guidelines established by the provider.
	char iptc_provincestate[MAX_EXIF_LEN];		/*! IPTC Province/State name, field Iptc.Application2.Provincestate */
//0x0064 	100 	Iptc.Application2.CountryCode 	String 	No 	No 	3 	3 	Indicates the code of the country/primary location where the intellectual property of the object data was created
	char iptc_countrycode[MAX_EXIF_LEN];		/*! IPTC City name, field Iptc.Application2.City */
//0x0065 	101 	Iptc.Application2.CountryName 	String 	No 	No 	0 	64 	Provides full
	char iptc_countryname[MAX_EXIF_LEN];		/*! IPTC City name, field Iptc.Application2.City */

	// Image processing data
	bool grayscaled;
	float sharpness;			/*! Sharpness factor in [0..100] */

	IplImage * thumbImage;		/*! Thumb image for faster display */
	IplImage * sharpnessImage;	/*! Sharpness image for faster display */
	IplImage * hsvImage;		/*! HSV histogram image for faster display */

	float log_histogram[3][256];	/*! Log histogram */

} t_image_info_struct;



/** @brief Image processing analyse class
  */
class ImageInfo {
public:
	ImageInfo();
	~ImageInfo();
	/** @brief Load input image */
	int loadFile(char * filename);

	/** @brief Return colored histogram */
	IplImage * getHistogram() { return m_HistoImage; };
	IplImage * getColorHistogram() { return m_ColorHistoImage; };
	IplImage * getSharpnessImage() { return m_sharpnessImage; };

	float getSharpness() { return m_image_info_struct.sharpness; };
	/** @brief Get structure containing every image information needed for sorting */
	t_image_info_struct getImageInfo() { return m_image_info_struct; };


private:
	/** @brief Initialization function */
	void init();
	/** @brief Desallocation function */
	void purge();

	/** @brief Read EXIF and IPTC metadata in image file */
	int readMetadata(char * filename);

	/** @brief Structure containing every image information needed for sorting */
	t_image_info_struct m_image_info_struct;

	/** @brief Original image */
	IplImage * m_originalImage;

	/** @brief Histogram */
	IplImage * m_HistoImage;

	/** @brief Scaled version of original image */
	IplImage * m_scaledImage;
	/*! Thumb image for faster display */
	IplImage * m_thumbImage;
	/** @brief Scaled & grayscaled version of original image */
	IplImage * m_grayImage;

	/** @brief Scaled & HSV version of original image */
	IplImage * hsvImage;
	/** @brief Scaled & HSV version of original image, H plane */
	IplImage * h_plane;
	/** @brief Scaled & HSV version of original image, S plane */
	IplImage * s_plane;

	/** @brief Scaled & RGB version of original image, RGBA planes array */
	IplImage * rgb_plane[4];

	/** @brief Sharpness image */
	IplImage * m_sharpnessImage;

	/** @brief HSV output image */
	IplImage * hsvOutImage;

	/** @brief purge scaled images */
	void purgeScaled();

	/** @brief Process HSV analysis */
	int processHSV();

	/** @brief Process RGB histogram analysis */
	int processRGB();

	/** @brief Process sharpness analysis */
	int processSharpness();



#define H_MAX	180
#define S_MAX	255
	IplImage * m_HSHistoImage;
	IplImage * m_ColorHistoImage;

//	unsigned long m_HSHistoImage[H_MAX][S_MAX];
};

#endif // IMAGEINFO_H

