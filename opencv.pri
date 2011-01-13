#this script try to locate opencv and set the right compilation options

# PG = opencv
# system(pkg-config --exists $$PG) {
#    message(opencv v$$system(pkg-config --modversion $$PG)found)
#    LIBS += $$system(pkg-config --libs $$PG)
#    INCLUDEPATH += $$system(pkg-config --cflags $$PG | sed s/-I//g)
# } else {
#   error($$PG "NOT FOUND => IT WILL NOT COMPILE")
#}

LIBSDIR =
LIBS_EXT = dylib


linux-g++: LIBS_EXT = so
unix: {
	##################### OPENCV >= 2.2 #####################
	# Test if OpenCV library is present
	exists( /usr/local/include/opencv2/opencv.hpp ) {
		message("OpenCV >= 2.2 found in /usr/local/include/opencv2/")
		DEFINES += OPENCV_22 OPENCV2
		INCLUDEPATH += /usr/local/include/opencv2

		CVINSTPATH = /usr/local
		CVINCPATH = /usr/local/include/opencv2

		LIBS += -L/usr/local/lib
		LIBSDIR = /usr/local/lib
	} else {
		exists( /usr/include/opencv2/opencv.hpp )
		{
			DEFINES += OPENCV_22 OPENCV2

			#message("OpenCV found in /usr/include.")
			CVINSTPATH = /usr
			CVINCPATH = /usr/include/opencv2
			INCLUDEPATH += /usr/include/opencv2

			LIBS += -L/usr/lib
			LIBSDIR = /usr/lib
		} else {
			##################### OPENCV <= 2.1 #####################
			# Test if OpenCV library is present
			exists( /usr/local/include/opencv/cv.hpp ) {
				DEFINES += OPENCV_21 OPENCV2
				#message("OpenCV found in /usr/local/include.")
				INCLUDEPATH += /usr/local/include/opencv

				CVINSTPATH = /usr/local
				CVINCPATH = /usr/local/include/opencv

				LIBS += -L/usr/local/lib
				LIBSDIR = /usr/local/lib
			} else {
				exists( /usr/include/opencv/cv.hpp )
				{
					DEFINES += OPENCV_21 OPENCV2
					#message("OpenCV found in /usr/include.")
					CVINSTPATH = /usr
					CVINCPATH = /usr/include/opencv
					INCLUDEPATH += /usr/include/opencv

					LIBS += -L/usr/lib
					LIBSDIR = /usr/lib
				} else {
					message ( "OpenCV NOT FOUND => IT WILL NOT COMPILE" )
				}
			}
		}
	}




	CV22_LIB = $$LIBSDIR/libopencv_core.$$LIBS_EXT
	message ( Testing CV lib = '$$CV22_LIB )
	exists( $$CV22_LIB ) {
		#message( " => Linking with -lcv ('$$CV_LIB' exists)")
		LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui
	} else {
		# For Ubuntu 7.10 for example, the link option is -lcv instead of -lopencv
		CV_LIB = $$LIBSDIR/libcv.$$LIBS_EXT
		#message ( Testing CV lib = '$$CV_LIB' )
		exists( $$CV_LIB ) {
			#message( " => Linking with -lcv ('$$CV_LIB' exists)")
			LIBS += -lcv -lcvaux -lhighgui
		} else {
			# on MacOS X with OpenCV 1, we must also link with cxcore
			#message( Dynamic libraries : '$$LIBS_EXT' )
			CXCORE_LIB = $$CVINSTPATH/lib/libcxcore.$$LIBS_EXT
			#message ( Testing CxCore lib = '$$CXCORE_LIB' )
			exists( $$CXCORE_LIB ) {
				#                      message( " => Linking with CxCore in '$$CVINSTPATH' ")
				LIBS += -lcxcore
			}

			CV_LIB = $$LIBSDIR/libopencv.$$LIBS_EXT
			exists($$CV_LIB ) {

				#message( " => Linking with -lopencv ('$$CV_LIB' does not exist)")
				LIBS += -lopencv -lcvaux -lhighgui
			}
		}
	}
}




