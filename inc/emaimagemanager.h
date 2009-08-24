/***************************************************************************
 *            emaimagemanager.h
 *		Manage all images info, cache and thumbnails
 *
 *  Sun Aug 16 19:32:41 2009
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

#ifndef EMAIMAGEMANAGER_H
#define EMAIMAGEMANAGER_H

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QList>
#include <QtCore/QStringList>

#include "imageinfo.h"

class EmaImageManager : public QThread
{
public:
	/** @brief Manager constructor */
	EmaImageManager();
	/** @brief Manager destructor */
	~EmaImageManager();
private:
	/** @brief Initialize memory and managed structures */
	void init();
	/** @brief Purge memory and managed structures */
	void purge();
	// ============= Thread management =============

public:
	/** @brief Return processing process in [0..100]
	@return 100 if done
	*/
	int getProgress() { return m_progress; };

	/** @brief Procesisng thread */
	virtual void run();

private:
	/** Run command */
	bool m_run;

	/** Run flag */
	bool m_running;

	/** @brief Processing progress in percent */
	int m_progress;

	QMutex mutex;
	QWaitCondition waitCond;

	QStringList m_appendFileList;
	QStringList m_removeFileList;

	// ============= Image list management =============
public:
	/** @brief Append a file list to managed pictures
		@return <0 if error, 0 if ok
	*/
	int appendFileList(QStringList filelist);
	/** @brief Append a file to managed pictures
		@return <0 if error, 0 if ok
	*/
	int appendFile(QString filename);

	/** @brief Remove a file to managed pictures
		@return <0 if error, 0 if ok
	*/
	int removeFile(QString filename);

	// ============== MANAGED FILES ======================
	/** @brief Return information about one image
		@return NULL if not loaded
	*/
	t_image_info_struct * getInfo(QString filename);


private:
	/** @brief Known information about images */
	QList<t_image_info_struct *> m_managedInfo;
	/** @brief Known images files list
		Files added to m_appendFileList are moved to m_managedFileList
		Files added to m_removeFileList are removed from m_managedFileList
	*/
	QStringList m_managedFileList;

	/** @brief Image processing class to extract info about image */
	ImageInfo m_imgProc;


};

/** @brief global image manager class */
EmaImageManager * emaMngr();

#endif // EMAIMAGEMANAGER_H
