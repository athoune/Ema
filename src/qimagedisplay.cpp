/***************************************************************************
 *            qimagedisplay.cpp
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
 
#include "qimagedisplay.h"
#include <qevent.h>
#include <stdio.h>

unsigned char g_debug_QImageDisplay = 0;

QImageDisplay::QImageDisplay(QWidget * l_parent)
	: QLabel(l_parent) {
	setMouseTracking(false);
}

void QImageDisplay::mousePressEvent(QMouseEvent * e)
{
	if(g_debug_QImageDisplay)
		fprintf(stderr, "QImageDisplay::%s:%d : e=%p\n", __func__, __LINE__, e);
	emit signalMousePressEvent(e);
}

void QImageDisplay::mouseReleaseEvent(QMouseEvent * e)
{
	if(g_debug_QImageDisplay)
			fprintf(stderr, "QImageDisplay::%s:%d : e=%p\n", __func__, __LINE__, e);
	emit signalMouseReleaseEvent(e);
}

void QImageDisplay::mouseMoveEvent(QMouseEvent * e)
{
	if(g_debug_QImageDisplay)
		fprintf(stderr, "QImageDisplay::%s:%d : e=%p\n", __func__, __LINE__, e);
	emit signalMouseMoveEvent(e);
}

void QImageDisplay::wheelEvent(QWheelEvent * e)
{
	if(e) {
		int numDegrees = e->delta() / 8;
		int numSteps = numDegrees / 15;

		if(g_debug_QImageDisplay) {
			fprintf(stderr, "QImageDisplay::%s:%d : Wheel event e=%p delta=%d\n",
					__func__, __LINE__, e, e->delta());
			fprintf(stderr, "\tDeplacement :\t%d steps = %d degrees\n",
					numSteps, numDegrees );
			fprintf(stderr, "\tOrientation :\t");
			switch(e->orientation()) {
			case Qt::Horizontal:
				fprintf(stderr, "Qt::Horizontal\n");
				break;
			case Qt::Vertical:
				fprintf(stderr, "Qt::Vertical\n");
				break;
			default:
				fprintf(stderr, "Unknown\n");
				break;
			}
		}
	}
	
	emit signalWheelEvent(e);
}
