#ifndef IMAGEINFOWIDGET_H
#define IMAGEINFOWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <cv.h>
#include <cv.hpp>
#include "imageinfo.h"

namespace Ui {
	class ImageInfoWidget;
}






/** @brief Information on image widget
*/
class ImageInfoWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(ImageInfoWidget)
public:
	explicit ImageInfoWidget(QWidget *parent = 0);
	virtual ~ImageInfoWidget();

	/** @brief Set the background image */
	void setImageFile(const QString &  imagePath);

	/** @brief Set image information of current image */
	void setImageInfo(t_image_info_struct * pinfo);

protected:
	virtual void changeEvent(QEvent *e);

private:
	Ui::ImageInfoWidget *m_ui;
	QLabel * m_starLabels[5];
	ImageInfo * m_imgProc;
};

#endif // IMAGEINFOWIDGET_H
