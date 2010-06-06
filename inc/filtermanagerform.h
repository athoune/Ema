/***************************************************************************
 *            filtermanagerform.h
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


#ifndef FILTERMANAGERFORM_H
#define FILTERMANAGERFORM_H

#include <QWidget>
#include <QResizeEvent>

namespace Ui {
    class FilterManagerForm;
}

class FilterManagerForm : public QWidget {
    Q_OBJECT
public:
    FilterManagerForm(QWidget *parent = 0);
    ~FilterManagerForm();

protected:
    void changeEvent(QEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

private:
    Ui::FilterManagerForm *ui;

signals:
	void signal_resizeEvent(QResizeEvent *e);
};

#endif // FILTERMANAGERFORM_H
