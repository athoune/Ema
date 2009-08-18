# -------------------------------------------------
# Project created by QtCreator 2009-07-01T21:37:51
# -------------------------------------------------
TEMPLATE = app

# Use lowercase name for Linux
TARGET = ema
CONFIG += debug

# icon
# reference : file:///usr/share/qt4/doc/html/appicon.html
# mac::ICON = icon/Ema.icns
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4

# and an uppercase first letter for Mac & Windows
mac::TARGET = Ema
win32::TARGET = Ema
include(opencv.pri)
LIBS += -lexiv2
DEPENDPATH += . \
    inc \
    src \
    ui
INCLUDEPATH += . \
    inc \
    ui
OBJECTS_DIR = .obj-simple
DEFINES += SIMPLE_VIEW \
    QT3_SUPPORT
TRANSLATIONS = photogeny_French.ts
SOURCES += src/main.cpp \
    src/navimagewidget.cpp \
    src/thumbimagewidget.cpp \
    src/searchcriterionwidget.cpp \
    src/qimagedisplay.cpp \
    src/imageinfowidget.cpp \
    src/mainimagewidget.cpp \
    src/emamainwindow.cpp \
    src/thumbimageframe.cpp \
    src/imageinfo.cpp \
    src/imgutils.cpp \
	src/exifdisplayscrollarea.cpp \
	src/metadatawidget.cpp \
	src/emaimagemanager.cpp
HEADERS += inc/navimagewidget.h \
    inc/thumbimagewidget.h \
    inc/searchcriterionwidget.h \
    inc/qimagedisplay.h \
    inc/imageinfowidget.h \
    inc/mainimagewidget.h \
    inc/emamainwindow.h \
    inc/thumbimageframe.h \
    inc/imageinfo.h \
    inc/imgutils.h \
	inc/exifdisplayscrollarea.h \
	inc/metadatawidget.h \
	inc/emaimagemanager.h
FORMS += ui/navimagewidget.ui \
    ui/thumbimagewidget.ui \
    ui/searchcriterionwidget.ui \
    ui/imageinfowidget.ui \
    ui/mainimagewidget.ui \
    ui/emamainwindow.ui \
    ui/thumbimageframe.ui \
	ui/exifdisplayscrollarea.ui \
	ui/metadatawidget.ui
RESOURCES += ema.qrc

# # INSTALLATION
# INSTALLS += target
# FINAL CONFIGURATION ==================================================
message( "")
message( "")
message( "FINAL CONFIGURATION ================================================== ")
message( "Configuration : ")
message( " config : $$CONFIG ")
message( " defs : $$DEFINES ")
message( " libs : $$LIBS ")
message( "FINAL CONFIGURATION ================================================== ")
message( "")
message( "")
OTHER_FILES += doc/OpenCVToolsForPhoto.txt
