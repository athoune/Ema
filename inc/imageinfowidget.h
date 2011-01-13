/***************************************************************************
 *            imageinfowidget.cpp : display information by image processing
 *
 *  Wed Jun 11 08:47:41 2009
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

#ifndef IMAGEINFOWIDGET_H
#define IMAGEINFOWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

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
	QPushButton * m_starButtons[6];
	ImageInfo * m_imgProc;
	/// Current info displayed
	t_image_info_struct * m_curInfo;

	/** @brief Manually force score of a picture */
	void forceScore(int percent);
private slots:
	/// Black flag => score 0
	void on_scoreButton0_clicked();
	void on_scoreButton1_clicked();
	void on_scoreButton2_clicked();
	void on_scoreButton3_clicked();
	void on_scoreButton4_clicked();
	void on_scoreButton5_clicked();
};

#endif // IMAGEINFOWIDGET_H
