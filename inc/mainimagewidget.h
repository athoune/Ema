#ifndef MAINIMAGEWIDGET_H
#define MAINIMAGEWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
	class MainImageWidget;
}

class MainImageWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(MainImageWidget)
public:
	explicit MainImageWidget(QWidget *parent = 0);
	virtual ~MainImageWidget();
	/** @brief Set the background image */
	void setImageFile(const QString &  imagePath);
	/** @brief Zoom on a part of input image at a specified scale */
	void zoomOn(int unscaled_x, int unscaled_y, int scale);
	/** @brief crop absolute part of image for display */
	void cropAbsolute(int crop_x, int crop_, int scale);

protected:
	virtual void changeEvent(QEvent *e);

	QRect m_displayRect;
	QImage m_fullImage;
	QImage m_displayImage;

	int m_zoom_scale;
	QPoint m_lastClick;
	QRect m_cropRect;
	QRect m_lastCrop;

	bool m_mouse_has_moved;
private slots:
	// Zoom options
	void on_globalImageLabel_signalMousePressEvent(QMouseEvent * e);
	void on_globalImageLabel_signalMouseReleaseEvent(QMouseEvent * e);
	void on_globalImageLabel_signalMouseMoveEvent(QMouseEvent * e);
	void on_globalImageLabel_signalWheelEvent( QWheelEvent * e );
private:
	Ui::MainImageWidget *m_ui;
};

#endif // MAINIMAGEWIDGET_H
