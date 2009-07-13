/***************************************************************************
 *  thumbimageframe - Thumb display picture
 *
 *  Jul 2 21:10:56 2009
 *  Copyright  2007  Christophe Seyve
 *  Email cseyve@free.fr
 ****************************************************************************/


#include "thumbimageframe.h"
#include "ui_thumbimageframe.h"
#include <QToolTip>

ThumbImageFrame::ThumbImageFrame(QWidget *parent) :
	QFrame(parent),
	m_ui(new Ui::ThumbImageFrame)
{
	m_ui->setupUi(this);
}

ThumbImageFrame::~ThumbImageFrame()
{
	delete m_ui;
}

void ThumbImageFrame::changeEvent(QEvent *e)
{
	QFrame::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void ThumbImageFrame::setImageFile(const QString & imagePath)
{
	// "NavImageWidget::%s:%d ('%s')\n",
	//		__func__, __LINE__,
	//		imagePath);
	QImage l_displayImage;// local only
	QImage fullImage(imagePath);
	if(fullImage.isNull()) {
		l_displayImage.fill(127);
		m_imagePath = "";
	}
	else {
		int wdisp = m_ui->globalImageLabel->width()-2;
		int hdisp = m_ui->globalImageLabel->height()-2;
		m_imagePath = imagePath;
		l_displayImage = fullImage.scaled( wdisp, hdisp,
								Qt::KeepAspectRatio );
	}
	m_ui->globalImageLabel->setPixmap(QPixmap::fromImage(l_displayImage));

	QToolTip::add(m_ui->globalImageLabel, imagePath);

}

void ThumbImageFrame::on_globalImageLabel_signalMousePressEvent(QMouseEvent * e) {
	emit signalThumbClicked(m_imagePath);
}
