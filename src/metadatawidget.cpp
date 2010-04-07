

#include "metadatawidget.h"
#include "ui_metadatawidget.h"

// Read metadata
#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>


MetadataWidget::MetadataWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MetadataWidget)
{
    m_ui->setupUi(this);
}

MetadataWidget::~MetadataWidget()
{
    delete m_ui;
}

void MetadataWidget::changeEvent(QEvent *e)
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

void MetadataWidget::setImageFile(const QString & fileName) {
	m_fileName = fileName;
}

void MetadataWidget::setImageInfo(t_image_info_struct * p_image_info_struct)
{
	if(!p_image_info_struct) return;
	QString displayStr;

	displayStr.sprintf("%s", p_image_info_struct->model);
	m_ui->makerLineEdit->setText(displayStr);
//	strcpy(m_image_info_struct.maker, displayStr.ascii());

	// Orientation
	//exifMaker = exifData["Exif.Image.Orientation"]; str = exifMaker.toString();
	//displayStr = QString::fromStdString(str);
	//m_image_info_struct.orientation = (char)( displayStr.contains("0") ? 0 : 1);
	displayStr.sprintf("%d", (int)p_image_info_struct->orientation);
	m_ui->orientationLineEdit->setText(displayStr);

	// DateTime
	//exifMaker = exifData["Exif.Photo.DateTimeOriginal"]; str = exifMaker.toString();
	//displayStr = QString::fromStdString(str);
	//
	displayStr.sprintf("%s", p_image_info_struct->datetime);
	//strcpy(m_image_info_struct.datetime, displayStr.ascii());
	m_ui->dateLineEdit->setText(displayStr);

	// Focal
	//exifMaker = exifData["Exif.Photo.FocalLengthIn35mmFilm"]; str = exifMaker.toString();
	//displayStr = QString::fromStdString(str);
	if(p_image_info_struct->focal_mm > 0) {
		//exifMaker = exifData["Exif.Photo.FocalLength"]; str = exifMaker.toString();
		//	displayStr.sprintf("%d", (int)p_image_info_struct->orientation);displayStr = QString::fromStdString(str);
		displayStr.sprintf("%g", p_image_info_struct->focal_mm);

	} else if(p_image_info_struct->focal_eq135_mm > 0) {
		displayStr.sprintf("%g", p_image_info_struct->focal_eq135_mm );

		displayStr += tr("eq. 35mm");
	} else displayStr = "??";

	m_ui->focalLineEdit->setText(displayStr);

	// Aperture
	//exifMaker = exifData["Exif.Photo.FNumber"]; str = exifMaker.toString();
	//displayStr = QString::fromStdString(str);
	//m_image_info_struct.aperture = rational_to_float(displayStr);
	displayStr.sprintf("%g", p_image_info_struct->aperture );
	m_ui->apertureLineEdit->setText(displayStr);

	// Speed
	//exifMaker = exifData["Exif.Photo.ExposureTime"]; str = exifMaker.toString();
	//displayStr = QString::fromStdString(str);
	//m_image_info_struct.speed_s = rational_to_float(displayStr);
	if(p_image_info_struct->speed_s > 0) {
		if(p_image_info_struct->speed_s < 1.f)
			displayStr.sprintf("1/%d s", (int)(1.f/p_image_info_struct->speed_s) );
		else
			displayStr.sprintf("%g s", (p_image_info_struct->speed_s) );
	}
	else displayStr = "??";

	m_ui->speedLineEdit->setText(displayStr);

	// ISO
//	exifMaker = exifData["Exif.Photo.ISOSpeedRatings"]; str = exifMaker.toString();
	displayStr.sprintf("%d", p_image_info_struct->ISO);
//		displayStr = rational(displayStr);
//	m_image_info_struct.ISO = (int)rational_to_float(displayStr);
		m_ui->isoLineEdit->setText(displayStr);
}

