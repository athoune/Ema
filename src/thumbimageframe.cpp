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

void ThumbImageFrame::setImageFile(const QString & imagePath, IplImage * img, int score )
{
	// "NavImageWidget::%s:%d ('%s')\n",
	//		__func__, __LINE__,
	//		imagePath);
	QPixmap l_displayImage;// local only
	m_imagePath = "";
	int wdisp = m_ui->globalImageLabel->width()-2;
	int hdisp = m_ui->globalImageLabel->height()-2;

	if(score < 0) { // hide
		m_ui->starsLabel->hide();
	} else {
		score --;
		if(score > 4) score = 4;
		QString stars = "";
		if(score > 0)
			for(int star = 0; star < score; star++) {
				stars += "*";
			}
		else
			score = 0;// for adding 5 x .

		for(int star = score; star < 5; star++) {
			stars += ".";
		}

		m_ui->starsLabel->setText(stars);
	}
	QPixmap fullImage;
	if(img ) {

		fullImage = iplImageToQImage(img);
	} else {
		fprintf(stderr, "ThumbImageWidget::%s:%d no IplImage => has to load '%s'\n",
				__func__, __LINE__,
				imagePath.ascii() );
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

void ThumbImageFrame::on_globalImageLabel_signalMousePressEvent(QMouseEvent * ) {
	emit signalThumbClicked(m_imagePath);
}
void ThumbImageFrame::on_globalImageLabel_signalMouseMoveEvent(QMouseEvent * ) {
	emit signalThumbSelected(m_imagePath);
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

