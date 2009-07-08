/***************************************************************************
 *            qimagedisplay.h
 *
 *  Mon Jun 16 23:47:41 2008
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

#ifndef QIMAGEDISPLAY_H
#define QIMAGEDISPLAY_H

#include <QLabel>

#include <qevent.h>


/** @brief Replacement for QLabel to display images with mouse events handling
 * */
class QImageDisplay : public QLabel {
	Q_OBJECT
public:
	QImageDisplay(QWidget * l_parent = NULL);

private:
	void mouseReleaseEvent(QMouseEvent * e);
	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);
	void wheelEvent ( QWheelEvent * e );

signals:
	void signalMousePressEvent(QMouseEvent * e);
	void signalMouseReleaseEvent(QMouseEvent * e);
	void signalMouseMoveEvent(QMouseEvent * e);
	void signalWheelEvent( QWheelEvent * e );
};

#endif
