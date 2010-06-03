/***************************************************************************
 *            emamainwindow.h
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

#ifndef EMAMAINWINDOW_H
#define EMAMAINWINDOW_H

#include <QtCore/QTimer>

#include <QtGui/QMainWindow>
#include <QtGui/QTreeWidgetItem>

namespace Ui
{
	class EmaMainWindow;
}


class EmaMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	EmaMainWindow(QWidget *parent = 0);
	~EmaMainWindow();


private:

	Ui::EmaMainWindow *ui;

	/// Opened image paths
	QStringList m_imageList;
	QStringList m_appendFileList;
	QStringList m_removeFileList;

	// Thumbnails sorter
	int m_sorter_nbitems_per_row;


	/** @brief append a list of files to displayed files */
	void appendFileList(QStringList list);

	/** @brief remove a list of files to displayed files */
	void removeFileList(QStringList list);

	void appendThumbImage(QString fileName);
	void removeThumbImage(QString fileName);

private:
	QTimer m_timer;

public slots:
	void on_thumbImage_clicked(QString fileName);
	void on_thumbImage_selected(QString);
private slots:
	void on_filesShowCheckBox_stateChanged(int);
	void on_filesLoadButton_clicked();
	void on_filesClearButton_clicked();
	void on_zoomx2Button_clicked();
	void on_zoomx1Button_clicked();
	void on_groupBox_7_clicked();

	void on_gridWidget_signal_resizeEvent(QResizeEvent *);

	// Collections
	void on_collecShowCheckBox_stateChanged(int);

	void on_timer_timeout();

	void on_gridButton_clicked();
	void on_imgButton_clicked();

	void on_mainGroupBox_resizeEvent(QResizeEvent *);
	//
	void on_globalNavImageWidget_signalZoomOn(int, int, int);

	void on_actionAbout_activated();

	void on_appendNewPictureThumb(const QString & );

	void on_filesTreeWidget_itemClicked ( QTreeWidgetItem * item, int column );
	void on_filesTreeWidget_itemDoubleClicked ( QTreeWidgetItem * item, int column );
};

#endif // EmaMAINWINDOW_H
