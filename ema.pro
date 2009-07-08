# -------------------------------------------------
# Project created by QtCreator 2009-07-01T21:37:51
# -------------------------------------------------
TEMPLATE = app

# Use lowercase name for Linux
TARGET = ema

# and an uppercase first letter for Mac & Windows
mac::TARGET = Ema
win32::TARGET = Ema
include(opencv.pri)
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
    src/imgutils.cpp
HEADERS += inc/navimagewidget.h \
    inc/thumbimagewidget.h \
    inc/searchcriterionwidget.h \
    inc/qimagedisplay.h \
    inc/imageinfowidget.h \
    inc/mainimagewidget.h \
    inc/emamainwindow.h \
    inc/thumbimageframe.h \
    inc/imageinfo.h \
    inc/imgutils.h
FORMS += ui/navimagewidget.ui \
    ui/thumbimagewidget.ui \
    ui/searchcriterionwidget.ui \
    ui/imageinfowidget.ui \
    ui/mainimagewidget.ui \
    ui/emamainwindow.ui \
    ui/thumbimageframe.ui
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
