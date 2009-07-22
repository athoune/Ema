#include "exifdisplayscrollarea.h"
#include "ui_exifdisplayscrollarea.h"

// Read metadata
#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>


ExifDisplayScrollArea::ExifDisplayScrollArea(QWidget *parent) :
    QScrollArea(parent),
    m_ui(new Ui::ExifDisplayScrollArea)
{
    m_ui->setupUi(this);
}

ExifDisplayScrollArea::~ExifDisplayScrollArea()
{
    delete m_ui;
}

void ExifDisplayScrollArea::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ExifDisplayScrollArea::setImageFile(const QString & fileName) {
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
		fprintf(stderr, "ExifDisplay::%s:%d : maker='%s'\n",
				__func__, __LINE__, displayStr.ascii());
		if(0)
		for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {

/*			str.sprintf("%d ", i->key());
			displayStr += QString(i->key()) ;
*/
			QString str; str = (i->tag());

			displayStr += str+ QString(" = ") + "\n";
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


//		m_ui->exifLabel->setText(displayStr);
		//m_ui->exifDataLabel->setText(tr("Hello"));
		return ;
	}
	catch (Exiv2::AnyError& e) {
		std::cout << "Caught Exiv2 exception '" << e << "'\n";
		return ;
	}

}
