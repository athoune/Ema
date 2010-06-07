/***************************************************************************
 *  thumbimagewidget - Thumb display picture with a few editing capabilities
 *
 *  Jul 2 21:10:56 2009
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <QToolTip>

#include "thumbimagewidget.h"
#include "ui_thumbimagewidget.h"

ThumbImageWidget::ThumbImageWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ThumbImageWidget)
{
    m_ui->setupUi(this);
}

ThumbImageWidget::~ThumbImageWidget()
{
    delete m_ui;
}

void ThumbImageWidget::changeEvent(QEvent *e)
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

void ThumbImageWidget::setImageFile(const QString & imagePath,
								   IplImage * img, int score )
{
	fprintf(stderr, "ThumbImageWidget::%s:%d ('%s', img=%p %dx%dx%d, score=%d)\n",
			__func__, __LINE__,
			imagePath.toUtf8().data(),
			img,
			img?img->width : -1,
			img?img->height : -1,
			img?img->nChannels : -1,
			score);

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
	if( img ) {
		QImage qImg = iplImageToQImage(img);
		fullImage = QPixmap::fromImage( qImg );
		fprintf(stderr, "ThumbImageWidget::%s:%d : load '%s' : "
				"iplImageToQImage(%dx%d) => qImg=%dx%d => pixmap=%dx%d\n",
				__func__, __LINE__,
				imagePath.toUtf8().data(),
				img->width, img->height,
				qImg.width(), qImg.height(),
				fullImage.width(), fullImage.height()
				);
	} else {
		fprintf(stderr, "ThumbImageWidget::%s:%d no IplImage => has to load '%s'\n",
				__func__, __LINE__,
				imagePath.toUtf8().data() );
		fullImage.load(imagePath);
	}

	if(fullImage.isNull()
		|| fullImage.width() == 0
		|| fullImage.height() == 0
		) {
		l_displayImage.fill(127);
	}
	else {
		m_imagePath = imagePath;
		l_displayImage = fullImage.scaled( wdisp, hdisp,
								Qt::KeepAspectRatio );
	}

	fprintf(stderr, "ThumbImageWidget::%s:%d : load '%s' : %dx%d => %dx%d\n",
			__func__, __LINE__,
			imagePath.toUtf8().data(),
			fullImage.width(), fullImage.height(),
			l_displayImage.width(), l_displayImage.height()
			);

	m_ui->globalImageLabel->setPixmap( l_displayImage );
	m_ui->globalImageLabel->setToolTip(imagePath);
}

// Forward mouse events
void ThumbImageWidget::mouseDoubleClickEvent ( QMouseEvent * event ) {
	// forward double click
	emit signal_mouseDoubleClickEvent ( event );

	// signal that we selected one item
//	emit signal_mouseDoubleClickFile ( m_filename );
}

void ThumbImageWidget::mouseMoveEvent ( QMouseEvent * event ) {
	emit signal_mouseMoveEvent ( event );
}
void ThumbImageWidget::mousePressEvent ( QMouseEvent * event ) {
	emit signal_mousePressEvent ( event );
}
void ThumbImageWidget::mouseReleaseEvent ( QMouseEvent * event ) {
	emit signal_mouseReleaseEvent ( event );
}
