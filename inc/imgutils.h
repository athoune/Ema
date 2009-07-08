/***************************************************************************
 *  imgutils - Basic utilities / image processing for dust detection
 *
 *  Sun Oct 28 14:10:56 2007
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


#ifndef IMGUTILS_H
#define IMGUTILS_H


#ifdef WIN32
#define tmsleep(s)	Sleep((s)*1000)
#define strcasestr	strstr
#include <sys/timeb.h>
#else // Unix
#define tmsleep(s)  sleep((s))
#endif

#include <stdint.h>


// For Open CV Functions
#include <cv.h>
#include <cv.hpp>
#include <cvaux.h>

#ifndef tmmin
#define tmmin(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef tmmax
#define tmmax(a,b) ((a)>(b)?(a):(b))
#endif

// DEBUG IMAGES DIRECTORY AND EXTENSION
#ifdef LINUX //not __X11_DARWIN__
#define TMP_DIRECTORY	"/dev/shm/"
#define IMG_EXTENSION	".pgm"
#else
#define TMP_DIRECTORY	"/Users/tof/tmp/"
#define IMG_EXTENSION	".jpg"
#endif

// TYPEDEFS FOR MULTIPLE ARCHITECTURE TYPES (32bit, 64bit...)
#ifndef u8
typedef unsigned char u8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
#endif

#ifdef IMGUTILS_CPP
#define IMGUTILS_EXTERN
#define IMGUTILS_NULL	=0
#else
#define IMGUTILS_EXTERN	extern
#define IMGUTILS_NULL
#endif

#define COLORMARK_THRESHOLD	251

//
#define COLORMARK_FAILED	252
#define COLORMARK_REFUSED	253
#define COLORMARK_CORRECTED	254
#define COLORMARK_CURRENT	255

// Difference image fill values

/// Difference image value for contour areas (a little different)
#define DIFF_CONTOUR    192

/// Difference image value for preventing from region growing (force copy)
#define DIFF_NEUTRALIZE 64

/// Difference image value for regions which have been considered as not being a dust
#define DIFF_NOT_DUST 128

/// Difference image value for regions which ARE considered as being a dust
#define DIFF_THRESHVAL  250

/// Flag to save debug images in @see TMP_DIRECTORY (activated with argument '-save')
IMGUTILS_EXTERN u8 g_debug_savetmp IMGUTILS_NULL;

// Gray level difference for _very_ visually different pixels
#define VISIBLE_DIFF	40


/** @brief Print image properties */
void tmPrintProperties(IplImage * img);

/** @brief Return image layer depth in bytes (e.g. 8bit jpeg is 1 byte, 16bit:2bytes...) */
int tmByteDepth(IplImage * iplImage);

/** @brief Create an IplImage width OpenCV's cvCreateImage and clear buffer */
IplImage * tmCreateImage(CvSize size, int depth, int channels);
/** @brief Release an image and clear pointer */
void tmReleaseImage(IplImage ** img);


/** @brief Copy an IplIMage into another */
void tmCopyImage(IplImage * img_src, IplImage * img_dest);


/** @brief Fast conversion from color/16bit image to 8bit grayscale image */
IplImage * tmFastConvertToGrayscale(IplImage * img);

/** @brief Process a dilatation */
void tmDilateImage(IplImage * src, IplImage * dst, int kernelsize = 1, int iterations = 1);

/** @brief Load a TIFF file in an OpenCV IplImage */
IplImage * tmOpenTiffImage(const char * filename, int * dpi);

/** @brief Load an OpenCV IplImage as file */
IplImage * tmLoadImage(const char * filename, int * dpi = NULL);

/** @brief Save an OpenCV IplImage as file */
void tmSaveImage(const char * filename, IplImage * src);

/** @brief Do a contrained region growing around a seed with thresholding */
void tmGrowRegion(unsigned char * growIn, unsigned char * growOut,
	int swidth, int sheight,
	int c, int r,
	unsigned char threshold,
	unsigned char fillValue,
	CvConnectedComp * areaOut);

/** @brief Do a contrained region growing around a seed with similar value */
void tmFloodRegion(unsigned char * growIn, unsigned char * growOut,
	int swidth, int sheight,
	int c, int r,
	unsigned char seedValue,
	unsigned char threshold,
	unsigned char fillValue,
	CvConnectedComp * areaOut);

/** @brief Erase a previoulsy grown region starting from a seed */
void tmEraseRegion(
	IplImage * grownImage,
	IplImage * diffImage,
	int c, int r,
	unsigned char fillValue);

/** @brief Crop an IplImage around a point in another IplImage
	@param[in] origImage input image, to be cropped
	@param[out] cropImage output image
	@param[in] crop_x coordinate of crop top-left point
	@param[in] crop_y coordinate of crop top-left point
	@param[in] threshold_false_colors threshold 8bit image for later false colors display (only on 8bit input images)
*/
void tmCropImage(IplImage * origImage, IplImage * cropImage, int crop_x, int crop_y, bool threshold_false_colors = false);

/** @brief Insert an IplImage into another IplImage
	@param[in] cropImage input image, to be cropped
	@param[out] destImage output image
	@param[in] insert_x coordinate of dest top-left point
	@param[in] insert_y coordinate of dest top-left point
*/
void tmInsertImage(IplImage * cropImage, IplImage * destImage, int insert_x, int insert_y);


/** @brief Return the ratio of pixels non 0 in an IplImage in a region */
float tmNonZeroRatio(IplImage * origImage, int x, int y, int w, int h,
			int exclu_x, int exclu_y, int exclu_w, int exclu_h,
			u8 threshold = DIFF_THRESHVAL);

/** @brief Add a border to image to reach a 4x size */
IplImage * tmAddBorder4x(IplImage * originalImage);

/** @brief Return correlation image */
IplImage * getCorrelationImage();



/** @brief Return correlation between 2 images
	@param[in] img1 input image 1
	@param[in] img2 input image 2
	@param[in] maskImage mask image 1
	@param[in] x1 input top-left corner in img 1
	@param[in] y1 input top-left corner in img 1
	@param[in] x2 input top-left corner in img 2
	@param[in] y2 input top-left corner in img 2
	@param[in] width width of correlation rectangle
	@param[in] height height of correlation rectangle
	@param[in] difftolerance maximum difference between images 1 and 2 : if difference > difftolerance, function returns with huge value

	@param[out] pmaxdiff maximum difference (mean difference)
	@param[out] pnbdiff number of pixels with visible difference (difference > VISIBLE_DIFF)

	@return correlation value (the smaller, the better)
*/
float tmCorrelation(
	IplImage * img1, IplImage * img2,
	IplImage * maskImage,
	int x1, int y1,
	int x2, int y2,
	int w, int h,
	int difftolerance,
	int * pmaxdiff,
	int * pnbdiff = NULL
	);

/** @brief Find a neighbour region to copy on the dust

	@param[in] origImage original image
	@param[in] maskImage dust growing mask image. If pixels of the mask have DIFF_THRESHVAL value, pixels are not considered.
	@param[in] orig_x center of the dust
	@param[in] orig_y center of the dust
	@param[in] orig_width width of the dust
	@param[in] orig_height width of the dust

	@param[out] copy_dest_x center of destination of copy
	@param[out] copy_dest_y center of destination of copy
	@param[out] copy_src_x center of source of copy
	@param[out] copy_src_y center of source of copy
	@param[out] copy_width width of copy
	@param[out] copy_height height of copy
	@param[out] best_correlation best correlation between source and destination

	@return 1 if found, 0 if no matching region found
*/
int tmSearchBestCorrelation(
	IplImage * origImage, IplImage * maskImage,
	int orig_x, int orig_y,
	int orig_width, int orig_height,
	// Outputs
	int * copy_dest_x, int * copy_dest_y,
	int * copy_src_x, int * copy_src_y,
	int * copy_width, int * copy_height,
	int * best_correlation
	);

/** @brief Copy a part of an image into itself ('clone tool')
	@param[in] origImage original image
	@param[in] dest_x center of destination of copy
	@param[in] dest_y center of destination of copy
	@param[in] src_x center of source of copy
	@param[in] src_y center of source of copy
	@param[in] copy_width width of copy
	@param[in] copy_height height of copy

	@param[out] destImage destination image, if NULL, the destination is the same as origin
*/
void tmCloneRegion(IplImage * origImage,
	int dest_x, int dest_y,
	int src_x, int src_y,
	int copy_width, int copy_height,
	IplImage * destImage = NULL);

/** @brief Copy a part of an image into itself ('clone tool')
	@param[in] origImage original image
	@param[in] dest_x top-left corner of destination of copy
	@param[in] dest_y top-left corner of destination of copy
	@param[in] src_x top-left corner of source of copy
	@param[in] src_y top-left corner of source of copy
	@param[in] copy_width width of copy
	@param[in] copy_height height of copy

	@param[out] destImage destination image, if NULL, the destination is the same as origin
*/
void tmCloneRegionTopLeft(IplImage * origImage,
	int dest_x, int dest_y,
	int src_x, int src_y,
	int copy_width, int copy_height,
	IplImage * destImage = NULL );


/** @brief Clear a part of an image ('erase tool')

*/
void tmFillRegion(IplImage * origImage,
	int center_x, int center_y,
	int fill_width, int fill_height,
	u8 fillValue);

/** @brief Mark the copy action from a part of an image into itself ('clone tool') */
void tmMarkCloneRegion(IplImage * origImage,
	int orig_x, int orig_y,
	int copy_x, int copy_y,
	int copy_width, int copy_height,
	bool mark = false);

/** @brief Mark failure as a gray region */
void tmMarkFailureRegion(IplImage * origImage,
	int x, int orig_y, int w, int h, unsigned char color = 127);

/** @brief Allocate a morphology structural element */
IplConvKernel * tmCreateStructElt(int size=3);

/** @brief Process a close morphology operation */
void tmCloseImage(IplImage * src, IplImage * dst, IplImage * tmp, int iterations);
/** @brief Process an open morphology operation */
void tmOpenImage(IplImage * src, IplImage * dst, IplImage * tmp, int iterations);

#define FILM_UNDEFINED	0
#define FILM_NEGATIVE	1
#define FILM_POSITIVE	2


/** @brief Process difference between original image and blurred image, depending on film type
	@param[in] l_FilmType film type (0=FILM_UNDEFINED, 1=FILM_NEGATIVE, 2=FILM_POSITIVE)
	@param[in] original (input) image
	@param[in] blurred image
	@param[out] difference image = output
	@param[out] variance image = output
	@param[out] diffHisto difference histogram (may be NULL)
	@param[out] variance neighbour size
*/
int tmProcessDiff(int l_FilmType, IplImage * grayImage,  IplImage * medianImage,
	IplImage * diffImage, IplImage * varianceImage = NULL,
	unsigned long * diffHisto = NULL, int var_size = 3);

#endif
