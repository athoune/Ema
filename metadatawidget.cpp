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

void MetadataWidget::setImageFile(const QString & fileName) {
	m_fileName = fileName;

	// Read metadata
	try {

		Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((char *)fileName.ascii());
		assert(image.get() != 0);
		image->readMetadata();

		Exiv2::ExifData &exifData = image->exifData();
		if (exifData.empty()) {
			std::string error(fileName);
			error += ": No Exif data found in the file";
			throw Exiv2::Error(1, error);
		}
		Exiv2::ExifData::const_iterator end = exifData.end();

		QString displayStr;

		Exiv2::Exifdatum& exifMaker = exifData["Exif.Image.Make"];
		std::string str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		exifMaker = exifData["Exif.Image.Model"];str = exifMaker.toString();
		displayStr += " / " + QString::fromStdString(str);
		m_ui->makerLineEdit->setText(displayStr);

		// Orientation
		exifMaker = exifData["Exif.Image.Orientation"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		m_ui->orientationLineEdit->setText(displayStr);

		// DateTime
		exifMaker = exifData["Exif.Photo.DateTimeOriginal"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
				m_ui->dateLineEdit->setText(displayStr);
		// Aperture

		exifMaker = exifData["Exif.Photo.FocalLengthIn35mmFilm"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		if(QString::compare(displayStr, "0")) {
			exifMaker = exifData["Exif.Photo.FocalLength"]; str = exifMaker.toString();
			displayStr = QString::fromStdString(str);
			displayStr = rational(displayStr);
		} else {
			displayStr = rational(displayStr);
			displayStr += tr("eq. 35mm");
		}
		m_ui->focalLineEdit->setText(displayStr);

		// Aperture
		exifMaker = exifData["Exif.Photo.FNumber"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		displayStr = rational(displayStr);
		m_ui->apertureLineEdit->setText(displayStr);
		// Speed
		exifMaker = exifData["Exif.Photo.ExposureTime"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		displayStr = rational_1_div(displayStr);
		m_ui->speedLineEdit->setText(displayStr);
		// Speed
		exifMaker = exifData["Exif.Photo.ISOSpeedRatings"]; str = exifMaker.toString();
		displayStr = QString::fromStdString(str);
		displayStr = rational(displayStr);
		m_ui->isoLineEdit->setText(displayStr);


		fprintf(stderr, "ExifDisplay::%s:%d : maker='%s'\n",
				__func__, __LINE__, displayStr.ascii());

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


		return ;
	}
	catch (Exiv2::AnyError& e) {
		std::cout << "Caught Exiv2 exception '" << e << "'\n";
		return ;
	}

}
