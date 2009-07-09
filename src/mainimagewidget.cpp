#include "mainimagewidget.h"
#include "ui_mainimagewidget.h"

MainImageWidget::MainImageWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::MainImageWidget)
{
	m_mouse_has_moved = false;
	m_ui->setupUi(this);
}

MainImageWidget::~MainImageWidget()
{
	delete m_ui;
}

void MainImageWidget::changeEvent(QEvent *e)
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

void MainImageWidget::setImageFile(const QString & imagePath)
{
	// "NavImageWidget::%s:%d ('%s')\n",
	//		__func__, __LINE__,
	//		imagePath);

	m_fullImage.load(imagePath);

	zoomOn(0,0, 0);

}

void  MainImageWidget::zoomOn(int x, int y, int scale) {
	if(m_fullImage.isNull()) { return; }


	int wdisp = m_ui->globalImageLabel->width()-2;
	int hdisp = m_ui->globalImageLabel->height()-2;

	int x_crop = (scale>0? x - wdisp/(scale*2) : 0);
	if(x_crop<0) x_crop = 0;
	else if(x_crop>m_fullImage.width() - wdisp) x_crop = m_fullImage.width() - wdisp;
	int y_crop = (scale>0? y - hdisp/(scale*2) : 0);
	if(y_crop<0) y_crop = 0;
	else if(y_crop>m_fullImage.height() - hdisp) y_crop = m_fullImage.height() - hdisp;

	cropAbsolute(x_crop, y_crop, scale);
}

void MainImageWidget::cropAbsolute(int x_crop, int y_crop, int scale)
{
	int wdisp = m_ui->globalImageLabel->width()-2;
	int hdisp = m_ui->globalImageLabel->height()-2;

	// Crop on center of image
	if(scale > 1) {
		QImage cropImage = m_fullImage.copy(
			x_crop, y_crop,
			wdisp/scale, hdisp/scale);

		m_displayImage = cropImage.scaled( wdisp, hdisp,
							Qt::KeepAspectRatio );
	} else if(scale == 1)
	{
		m_displayImage = m_fullImage.copy(
			x_crop, y_crop,
			wdisp, hdisp);
	}
	m_cropRect = QRect(x_crop, y_crop, m_displayImage.width(), m_displayImage.height());

	if(scale <= 0){
		scale = 0;
		m_displayImage = m_fullImage.scaled( wdisp, hdisp,
							Qt::KeepAspectRatio );
	}

	m_zoom_scale = scale;
	m_ui->globalImageLabel->setPixmap(QPixmap::fromImage(m_displayImage));
}

void MainImageWidget::on_globalImageLabel_signalMousePressEvent(QMouseEvent * e) {


	fprintf(stderr, "MainImageWidget::%s:%d : m_lastClick=%d,%d\n", __func__, __LINE__,
			m_lastClick.x(), m_lastClick.y());
	m_mouse_has_moved = false;

	if(m_zoom_scale == 0) // toggle view
	{
		m_zoom_scale = 1;
		m_mouse_has_moved = true;

		int x = e->pos().x(), y =  e->pos().y();
		x *= m_fullImage.width() / m_displayImage.width();
		y *= m_fullImage.height() / m_displayImage.height();

		zoomOn(x, y, m_zoom_scale);
	}
	else {

		if(!m_mouse_has_moved)
		{
			m_lastClick = e->pos();
			m_lastCrop = m_cropRect;
		}
	}
}

void MainImageWidget::on_globalImageLabel_signalMouseReleaseEvent(QMouseEvent * e) {
	if(m_fullImage.isNull()) { return; }
	if(!e) return;


	if(m_zoom_scale == 0) // toggle view
	{
		m_zoom_scale = 1;
	} else if(!m_mouse_has_moved ){
		m_zoom_scale = 0;

		int x = m_fullImage.width() / 2;
		int y = m_fullImage.height() / 2;

		zoomOn(x, y, m_zoom_scale);
	}
}

void MainImageWidget::on_globalImageLabel_signalMouseMoveEvent(QMouseEvent * e) {
	if(!e) return;
	int dx = e->pos().x() - m_lastClick.x();
	int dy = e->pos().y() - m_lastClick.y();

	if(m_zoom_scale != 0) {
		dx /= m_zoom_scale;
		dy /= m_zoom_scale;
	}

	//fprintf(stderr, "MainImageWidget::%s:%d : dx,dy=%d,%d\n", __func__, __LINE__, dx, dy);
	m_mouse_has_moved = (dx != 0) || (dy != 0);

	cropAbsolute(m_lastCrop.x() - dx ,
		   m_lastCrop.y() - dy,
		   m_zoom_scale);
}

void MainImageWidget::on_globalImageLabel_signalWheelEvent( QWheelEvent * e )
{
	if(!e) return;
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	m_zoom_scale += numSteps;

	zoomOn(e->x(), e->y(), m_zoom_scale);
}
