/***************************************************************************
 *  metadatawidget - Metadata display widget for one picture
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

#ifndef METADATAWIDGET_H
#define METADATAWIDGET_H

#include <QtGui/QWidget>
#include "imageinfo.h"

namespace Ui {
    class MetadataWidget;
}

class MetadataWidget : public QWidget {
    Q_OBJECT
public:
    MetadataWidget(QWidget *parent = 0);
    ~MetadataWidget();
	void setImageFile(const QString & str);
	void setImageInfo(t_image_info_struct * p_image_info_struct);

private:
	QString m_fileName;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MetadataWidget *m_ui;
};

#endif // METADATAWIDGET_H
