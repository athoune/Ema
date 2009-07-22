#ifndef NAVIMAGEWIDGET_H
#define NAVIMAGEWIDGET_H

#include <QtGui/QWidget>
#include "qimagedisplay.h"

namespace Ui {
	class NavImageWidget;
}

/** @brief Global image navigation */
class NavImageWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(NavImageWidget)
public:
	explicit NavImageWidget(QWidget *parent = 0);
	virtual ~NavImageWidget();

	/** @brief Set the background image */
	void setImageFile(const QString &  imagePath);
	/** @brief Set the main display crop size  */
	void setMainDisplaySize(int x, int h);
protected:
	virtual void changeEvent(QEvent *e);

private:
	Ui::NavImageWidget *m_ui;

	QRect m_fullRect;
	QRect m_displayRect;
	QPixmap m_inputImage;
	QPixmap m_displayImage;

	/** @brief Zoom on a part of input image at a specified scale */
	void zoomOn(int x, int y, int scale);
	int m_zoom_scale;
	QPoint m_lastClick;

private slots:
	void on_zoomx2Button_released();
	void on_zoomx1Button_released();
	void on_zoomFitButton_clicked();
	// Zoom options
	void on_globalImageLabel_signalMousePressEvent(QMouseEvent * e);
	void on_globalImageLabel_signalMouseReleaseEvent(QMouseEvent * e);
	void on_globalImageLabel_signalMouseMoveEvent(QMouseEvent * e);
	void on_globalImageLabel_signalWheelEvent( QWheelEvent * e );
signals:
	void signalZoomOn(int, int, int);
};

#endif // NAVIMAGEWIDGET_H
