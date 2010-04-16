/***************************************************************************
 *            emaimagemanager.h
 *		Manage all images info, cache and thumbnails
 *
 *  Sun Aug 16 19:32:41 2009
 *  Copyright  2009  Christophe Seyve
 *  Email cseyve@free.fr
 ****************************************************************************/

#include "imgutils.h"
#include "emaimagemanager.h"

int g_EMAImgMng_debug_mode = EMALOG_DEBUG;

#define EMAIM_printf(a,...)  { \
		if(g_EMAImgMng_debug_mode>=(a)) { \
			fprintf(stderr,"EmaImageManager::%s:%d : ",__func__,__LINE__); \
			fprintf(stderr,__VA_ARGS__); \
			fprintf(stderr,"\n"); \
		} \
	}

EmaImageManager * g_EmaImageManager = NULL;

EmaImageManager * emaMngr() {
	if(!g_EmaImageManager) {
		EMAIM_printf(EMALOG_INFO, "Creating main manager...")
		g_EmaImageManager = new EmaImageManager();

		EMAIM_printf(EMALOG_INFO, "Starting main manager's thread")
		g_EmaImageManager->start();
	}

	return g_EmaImageManager;
}

EmaImageManager::EmaImageManager()
		: QThread()
{
	init();
}
void EmaImageManager::init()
{

}

EmaImageManager::~EmaImageManager()
{

}
void EmaImageManager::purge()
{

}


/* Append a file list to managed pictures */
int EmaImageManager::appendFileList(QStringList list) {

	m_progress = 0;
	// Process image
	QStringList::Iterator it = list.begin();

	QString fileName;
	int nb = list.count();
	EMAIM_printf(EMALOG_DEBUG, "Appending %d files...", nb)

	while(it != list.end()) {
		fileName = (*it);
		++it;
		m_appendFileList.append(fileName);
	}

	// Unlock thread
	mutex.lock();
	waitCond.wakeAll();
	mutex.unlock();

	return 0;
}

t_image_info_struct * EmaImageManager::getInfo(QString filename)
{
	if(!m_managedFileList.contains(filename)) {
		EMAIM_printf(EMALOG_TRACE, "File '%s' not found", filename.ascii());
		return NULL;
	}
	// Fill with managed
	int idx = m_managedFileList.indexOf(filename);
	if(idx < 0) {
		EMAIM_printf(EMALOG_TRACE, "File '%s' not found", filename.ascii())
		return NULL;
	}

	t_image_info_struct * pinfo = m_managedInfo.at(idx);
	if(!pinfo) return NULL;

	// Security : check filename
	if(!filename.contains(pinfo->filepath)) {
		EMAIM_printf(EMALOG_TRACE, "File '%s' mismatches pinfo ('%s')",
					 filename.ascii(),
					 pinfo->filepath)
		return NULL;
	}

	return pinfo ;
}
/* Append a file to managed pictures */
int EmaImageManager::appendFile(QString filename) {

	m_progress = 0;

	// if not already managed
	if(m_managedFileList.contains(filename)) {
		EMAIM_printf(EMALOG_DEBUG, "Already managed : '%s'", filename.ascii());
		return 0;
	}

	// Process image
	m_appendFileList.append(filename);
	// Unlock thread
	mutex.lock();
	waitCond.wakeAll();
	mutex.unlock();

	return 0;
}

/* Remove a file to managed pictures */
int EmaImageManager::removeFile(QString filename) {
	m_progress = 0;

	// if not already managed
	if(!m_managedFileList.contains(filename)) {
		EMAIM_printf(EMALOG_DEBUG, "Unknown / not managed : '%s'", filename.ascii());
		return 0;
	}

	m_removeFileList.append(filename);

	// Unlock thread
	mutex.lock();
	waitCond.wakeAll();
	mutex.unlock();

	return 0;
}




// =============================================================================
//				PROCESSING THREAD
// =============================================================================
void EmaImageManager::run() {
	m_run = true;
	m_running = true;


	while(m_run) {
		int wait_ms = 400;

		mutex.lock();
		waitCond.wait(&mutex, wait_ms);
		mutex.unlock();

		// Check if new files have been added
		if(!m_appendFileList.isEmpty()) {
			//
			// Process image info extraction, then add files
			QStringList::Iterator it = m_appendFileList.begin();

			QString fileName;
			int nb = m_appendFileList.count();
			EMAIM_printf(EMALOG_DEBUG, "Adding %d files...", nb)

			int cur = 0;
			while(it != m_appendFileList.end()) {
				fileName = (*it);
				++it;
				EMAIM_printf(EMALOG_DEBUG, "\tAdding file '%s'...", fileName.ascii())

				// Process info extraction
				m_imgProc.loadFile((char *)fileName.ascii());

				// Create a new storage
				t_image_info_struct l_info = m_imgProc.getImageInfo();
				if(l_info.valid) {
					t_image_info_struct * new_info = new t_image_info_struct;

					memcpy(new_info, &l_info, sizeof(t_image_info_struct));


					// Copy thumb
					new_info->thumbImage = tmClone(l_info.thumbImage);
					// Copy HSV histo
					new_info->hsvImage = tmClone(l_info.hsvImage);
					// Copy sharpness
					new_info->sharpnessImage = tmClone(l_info.sharpnessImage);

					// Copy RGB histo
//					new_info->histogram

					// append to managed list
					m_managedFileList.append(fileName);
					m_managedInfo.append(new_info);
				}

				cur++;
				m_progress = 100 * cur / nb;
			}
			m_appendFileList.clear(); // FIXME : protect with mutex or local copy
		}

	}

	m_running = false;
	EMAIM_printf(EMALOG_INFO, "Processing thread ended.")
}







