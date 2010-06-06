/***************************************************************************
 *  navimagewidget - Navigation display for main image
 *
 *  Jul 2 21:10:56 2009
 *  Copyright  2009  Christophe Seyve
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

#include "navimagewidget.h"
#include "ui_navimagewidget.h"

NavImageWidget::NavImageWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::NavImageWidget)
{
	m_zoom_scale = 0;
	m_ui->setupUi(this);
}

NavImageWidget::~NavImageWidget()
{
	delete m_ui;
}

void NavImageWidget::changeEvent(QEvent *e)
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


void NavImageWidget::on_zoomFitButton_clicked()
{
	m_zoom_scale = 0;
	if(m_displayImage.isNull()) { return; }
	emit signalZoomOn(0, 0, 0);
}

void NavImageWidget::on_zoomx1Button_released()
{
	m_zoom_scale = 1;
	emit signalZoomOn( m_fullRect.width()/2,
			m_fullRect.height()/2,
			1);
}

void NavImageWidget::on_zoomx2Button_released()
{
	m_zoom_scale = 2;
	emit signalZoomOn(m_fullRect.width()/2,
			m_fullRect.height()/2,
			2);
}

void NavImageWidget::setImageFile(const QString & imagePath)
{
	// "NavImageWidget::%s:%d ('%s')\n",
	//		__func__, __LINE__,
	//		imagePath);

	QPixmap fullImage(imagePath);
	if(fullImage.isNull()) {
		m_displayImage.fill(127);
		m_fullRect = QRect(0, 0, 0, 0);
		return;
	}
	else {
		int wdisp = m_ui->globalImageLabel->width()-2;
		int hdisp = m_ui->globalImageLabel->height()-2;
		m_fullRect = QRect(0, 0, fullImage.width(), fullImage.height());

		m_inputImage = m_displayImage = fullImage.scaled( wdisp, hdisp,
								Qt::KeepAspectRatio );

	}
	m_ui->globalImageLabel->setPixmap( m_displayImage );

	on_zoomFitButton_clicked();

}

void  NavImageWidget::zoomOn(int x, int y, int scale) {
	if(m_displayImage.isNull()) { return; }

	int center_x = x * m_fullRect.width() / m_displayImage.width();
	int center_y = x * m_fullRect.height() / m_displayImage.height();

	emit signalZoomOn(center_x, center_y, scale);

	//  display a rectangle
	m_zoom_scale = scale;

	m_ui->globalImageLabel->setPixmap(m_displayImage);
}

void NavImageWidget::on_globalImageLabel_signalMousePressEvent(QMouseEvent * e) {
	if(m_displayImage.isNull()) { return; }
	if(!e) return;

	if(m_zoom_scale == 0) // toggle view
	{
		// Select the zoomx1button
//		m_ui->zoomx1Button->set
		m_zoom_scale = 1;
	}

	int x = e->pos().x(), y =  e->pos().y();
	int glob_w = m_ui->globalImageLabel->width();
	int glob_h = m_ui->globalImageLabel->height();
	int disp_w = m_displayImage.width();
	int disp_h = m_displayImage.height();

	x = (x - (glob_w - disp_w)/2) * m_fullRect.width() / disp_w;
	y = (y - (glob_h - disp_h)/2) * m_fullRect.height() / disp_h;

	emit signalZoomOn(x, y, m_zoom_scale);
}

void NavImageWidget::on_globalImageLabel_signalMouseReleaseEvent(QMouseEvent * e) {
}

void NavImageWidget::on_globalImageLabel_signalMouseMoveEvent(QMouseEvent * e) {
	if(!e) return;
	int x = e->pos().x(), y =  e->pos().y();
	int glob_w = m_ui->globalImageLabel->width();
	int glob_h = m_ui->globalImageLabel->height();
	int disp_w = m_displayImage.width();
	int disp_h = m_displayImage.height();

	x = (x - (glob_w - disp_w)/2) * m_fullRect.width() / disp_w;
	y = (y - (glob_h - disp_h)/2) * m_fullRect.height() / disp_h;

	emit signalZoomOn(x, y, m_zoom_scale);
}

void NavImageWidget::on_globalImageLabel_signalWheelEvent( QWheelEvent * e )
{
	if(!e) return;
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	m_zoom_scale += numSteps;

	int x = e->pos().x(), y =  e->pos().y();
	int glob_w = m_ui->globalImageLabel->width();
	int glob_h = m_ui->globalImageLabel->height();
	int disp_w = m_displayImage.width();
	int disp_h = m_displayImage.height();

	x = (x - (glob_w - disp_w)/2) * m_fullRect.width() / disp_w;
	y = (y - (glob_h - disp_h)/2) * m_fullRect.height() / disp_h;

	emit signalZoomOn(x, y, m_zoom_scale);
}
