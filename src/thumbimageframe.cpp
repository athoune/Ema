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

void ThumbImageFrame::setImageFile(const QString & imagePath, IplImage * img )
{
	// "NavImageWidget::%s:%d ('%s')\n",
	//		__func__, __LINE__,
	//		imagePath);
	QPixmap l_displayImage;// local only
	m_imagePath = "";
	int wdisp = m_ui->globalImageLabel->width()-2;
	int hdisp = m_ui->globalImageLabel->height()-2;

	QPixmap fullImage;
	if(img ) {
		fullImage = iplImageToQImage(img);
	} else {
		fullImage.load(imagePath);
	}

	if(fullImage.isNull()) {
		l_displayImage.fill(127);
	}
	else {
		m_imagePath = imagePath;
		l_displayImage = fullImage.scaled( wdisp, hdisp,
								Qt::KeepAspectRatio );
	}
	m_ui->globalImageLabel->setPixmap( l_displayImage );

	QToolTip::add(m_ui->globalImageLabel, imagePath);
}

void ThumbImageFrame::on_globalImageLabel_signalMousePressEvent(QMouseEvent * e) {
	emit signalThumbClicked(m_imagePath);
}




// Forward mouse events
void ThumbImageFrame::mouseDoubleClickEvent ( QMouseEvent * event ) {
	// forward double click
	emit signal_mouseDoubleClickEvent ( event );

	// signal that we selected one item
	emit signal_mouseDoubleClickFile ( m_imagePath );
}

void ThumbImageFrame::mouseMoveEvent ( QMouseEvent * event ) {
	emit signal_mouseMoveEvent ( event );
}
void ThumbImageFrame::mousePressEvent ( QMouseEvent * event ) {
	emit signal_mousePressEvent ( event );
}
void ThumbImageFrame::mouseReleaseEvent ( QMouseEvent * event ) {
	emit signal_mouseReleaseEvent ( event );
}

