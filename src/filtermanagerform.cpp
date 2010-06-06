/***************************************************************************
 *            filtermanagerform.cpp
 *
 *  Wed Jun 2 23:47:41 2010
 *  Copyright  2010  Christophe Seyve
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



#include "filtermanagerform.h"
#include "ui_filtermanagerform.h"

#include "emaimagemanager.h"

#include "imgutils.h"
#include "imageinfo.h"


int g_EMAFILT_debug_mode = EMALOG_DEBUG;

#define EMAFILT_printf(a,...)  { \
				if(g_EMAFILT_debug_mode>=(a)) { \
					fprintf(stderr, "FilterManagerForm::%s:%d : ",__func__,__LINE__); \
					fprintf(stderr,__VA_ARGS__); \
					fprintf(stderr,"\n"); fflush(stderr); \
				} \
			}


FilterManagerForm::FilterManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterManagerForm)
{
    ui->setupUi(this);
}

FilterManagerForm::~FilterManagerForm()
{
    delete ui;
}

void FilterManagerForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FilterManagerForm::resizeEvent(QResizeEvent *e) {
	EMAFILT_printf(EMALOG_DEBUG, "Resize event : %dx%d",
				   e->size().width(),
				   e->size().height()
				   );

	emit signal_resizeEvent(e);
}
