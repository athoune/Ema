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
#ifndef IMAGEINFO_H
#define IMAGEINFO_H
#include <cv.h>
#include <cv.hpp>
#include <highgui.h>

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN	512
#endif

#define MAX_EXIF_LEN	32

/** @brief Useful information for sorting pictures*/
typedef struct {
	std::string filepath;
	// EXIF TAGS
	std::string maker;	/*! Company wich produces this camera */
	std::string model;	/*! Model of this camera */

	float focal_mm;				/*! Real focal in mm */
	float focal_eq125_mm;		/*! 135mm equivalent focal in mm (if available) */
	float aperture;				/*! F Number */
	float speed_s;				/*! Speed = shutter opening time in seconds */
	int ISO;					/*! ISO Sensitivity */

	// IPTC TAGS

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

	float getSharpness() { return m_sharpness; };

	t_image_info_struct getInfo() { return m_info; };

private:
	/** @brief Initialization function */
	void init();
	/** @brief Desallocation function */
	void purge();


	/** @brief Read metadata EXIF + IPTC */
	void readMetadata();

	/** @brief Information about image used to filter/search */
	t_image_info_struct m_info;

	/** @brief Original image */
	IplImage * m_originalImage;

	/** @brief Histogram */
	IplImage * m_HistoImage;

	/** @brief Scaled version of original image */
	IplImage * m_scaledImage;
	/** @brief Scaled & grayscaled version of original image */
	IplImage * m_grayImage;
	/** @brief Scaled & HSV version of original image */
	IplImage * hsvImage;
	IplImage * h_plane;
	IplImage * s_plane;
	IplImage * rgb_plane[4];

	/** @brief Sharpness image */
	IplImage * m_sharpnessImage;
	/** @brief Sharpness factor */
	float m_sharpness;

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
