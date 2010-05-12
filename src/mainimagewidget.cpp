/***************************************************************************
 *      Main image display in middle of GUI
 *
 *  Sun Aug 16 19:32:41 2009
 *  Copyright  2009  Christophe Seyve
 *  Email cseyve@free.fr
 ****************************************************************************/

#include "mainimagewidget.h"
#include "ui_mainimagewidget.h"

int g_EMAMainImgW_debug_mode = EMALOG_DEBUG;

#define EMAMIW_printf(a,...)  { \
		if(g_EMAMainImgW_debug_mode>=(a)) { \
			fprintf(stderr,"EmaMainImageW::%s:%d : ",__func__,__LINE__); \
			fprintf(stderr,__VA_ARGS__); \
			fprintf(stderr,"\n"); \
		} \
	}


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

void MainImageWidget::setImageFile(const QString & imagePath,
								   t_image_info_struct * pinfo )
{
	// "NavImageWidget::%s:%d ('%s')\n",
	//		__func__, __LINE__,
	//		imagePath);

	m_fullImage.load(imagePath);
	m_mouse_has_moved = false;
	m_lastClick	= QPoint(-1, -1);

	zoomOn(0,0, 0);

	if(!m_fullImage.isNull()) {
		char info[512];
		if(!pinfo) {
			sprintf(info, "%s\n%d x %d x %d\n",
					imagePath.ascii(),
					m_fullImage.width(),
					m_fullImage.height(),
					m_fullImage.depth()/8);
		} else {
			sprintf(info, "%s\n%d x %d x %d\n%d ISO\n%s",
					imagePath.ascii(),
					m_fullImage.width(),
					m_fullImage.height(),
					m_fullImage.depth()/8,
					pinfo->ISO,
					pinfo->datetime
					);

		}
		QString strInfo(info);
		m_ui->globalImageLabel->setToolTip(strInfo);
	}
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
fprintf(stderr, "MainImageWidget::%s:%d : crop %d,%d  sc=%d\n",
		__func__, __LINE__, x_crop, y_crop, scale);

	// Crop on center of image
	if(scale > 1) {
		QImage cropImage = m_fullImage.copy(
			x_crop, y_crop,
			wdisp/scale, hdisp/scale);

		m_displayImage = cropImage.scaled( wdisp, hdisp,
							Qt::KeepAspectRatio );
	}
	else if(scale == 1)
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
	int x = e->pos().x(), y =  e->pos().y();

	EMAMIW_printf(EMALOG_DEBUG, "m_lastClick=%d,%d zoom=%d click=%d,%d "
				  "/ display img=%dx%d / orig=%dx%d /widget=%dx%d",
			m_lastClick.x(), m_lastClick.y(),
			m_zoom_scale,
			x, y,

			m_displayImage.width(), m_displayImage.height(),
			m_fullImage.width(), m_fullImage.height(),
			width(), height()
			);

	m_mouse_has_moved = false;

	if(m_zoom_scale == 0) // toggle view to zoom 1:1
	{
		m_zoom_scale = 1;
		m_mouse_has_moved = true;
		float fx = e->pos().x() - (width()-m_displayImage.width())/2
				   , fy =  e->pos().y() - (height()-m_displayImage.height())/2;

		fx *= (float)m_fullImage.width() / (float)m_displayImage.width();
		fy *= (float)m_fullImage.height() / (float)m_displayImage.height();

		EMAMIW_printf(EMALOG_DEBUG, "click=%d,%d => in full=%g,%g"
					  "/ display img=%dx%d / orig=%dx%d",
					  x, y,
					  fx, fy,
					  m_displayImage.width(), m_displayImage.height(),
					  m_fullImage.width(), m_fullImage.height()
					  );

		x = (int)roundf(fx);
		y = (int)roundf(fy);
		m_lastClick = QPoint( x, y );

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
void MainImageWidget::on_actionZoomIn_activated( ) {
	fprintf(stderr, "MainImageW::%s:%d : zoom=%d",
			__func__, __LINE__, m_zoom_scale);
	m_zoom_scale ++;

	zoomOn(width()/2, height()/2, m_zoom_scale);

}
