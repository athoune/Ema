/***************************************************************************
 *            exifdisplayscrollarea.h
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

#ifndef EXIFDISPLAYSCROLLAREA_H
#define EXIFDISPLAYSCROLLAREA_H

#include <QtGui/QScrollArea>
#include "imageinfo.h"

namespace Ui {
    class ExifDisplayScrollArea;
}

class ExifDisplayScrollArea : public QScrollArea {
    Q_OBJECT
    Q_DISABLE_COPY(ExifDisplayScrollArea)
public:
    explicit ExifDisplayScrollArea(QWidget *parent = 0);
    virtual ~ExifDisplayScrollArea();
	//void setImageFile(const QString & str);
	void setImageInfo(t_image_info_struct * pinfo);
protected:
    virtual void changeEvent(QEvent *e);

private:
	QString m_fileName;
    Ui::ExifDisplayScrollArea *m_ui;
};

#endif // EXIFDISPLAYSCROLLAREA_H
