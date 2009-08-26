/***************************************************************************
 *            emamainwindow.cpp
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

//#include <stdlib.h>
//#include <stdio.h>


#include "emamainwindow.h"
#include "ui_emamainwindow.h"

#include "emaimagemanager.h"

#include "thumbimageframe.h"
#include "imgutils.h"
#include "imageinfo.h"

#include <QFileDialog>
#include <QSplashScreen>


int g_EMAMW_debug_mode = EMALOG_DEBUG;

#define EMAMW_printf(a,...)  { \
				if(g_EMAMW_debug_mode>=(a)) { \
					fprintf(stderr,"EmaMainWindow::%s:%d : ",__func__,__LINE__); \
					fprintf(stderr,__VA_ARGS__); \
					fprintf(stderr,"\n"); \
				} \
			}


EmaMainWindow::EmaMainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::EmaMainWindow)
{
	ui->setupUi(this);
	ui->loadProgressBar->hide();

	connect(&m_timer, SIGNAL(timeout()),
			this, SLOT(on_timer_timeout()));
}

EmaMainWindow::~EmaMainWindow()
{
	delete ui;
}

void EmaMainWindow::on_appendNewPictureThumb(const QString & filename) {
	// update progress
	appendThumbImage(filename);
}

void EmaMainWindow::on_gridButton_clicked() {
	//
	ui->stackedWidget->setCurrentIndex(1);
}

void EmaMainWindow::on_imgButton_clicked() {
	//
	ui->stackedWidget->setCurrentIndex(0);
}


QSplashScreen * g_splash = NULL;

void EmaMainWindow::on_actionAbout_activated() {
	QPixmap pix(":/icons/icons/ema-splash.png");
	if(g_splash) {
		g_splash->setPixmap(pix);
	}
	else {
		g_splash = new QSplashScreen(pix, Qt::WindowStaysOnTopHint );
	}

	QString verstr, cmd = QString("Ctrl+");
	QString item = QString("<li>"), itend = QString("</li>\n");
	g_splash->showMessage(

			QString("<br><br><br><br><br><br><br><br><br>") +
			QString("<br><br><br><br><br><br><br><br><br>") +
			QString("<br><br><br><br><br><br><br>") +
			tr("<b>Ema</b> version: ")

						  + verstr.sprintf("svn%04d%02d%02d", VERSION_YY, VERSION_MM, VERSION_DD)

						  + QString("<br>Website & Wiki: <a href=\"http://github.com/athoune/ema/\">http://github.com/athoune/ema/</a><br><br>")
						  + tr("Shortcuts :<br><ul>\n")
//							+ item + cmd + tr("O: Open a picture file") + itend
//							+ item + cmd + tr("S: Save corrected image") + itend
//							+ item + cmd + tr("H: Display version information") + itend
//							+ item + tr("M: Mark current crop area in red") + itend
//							+ item + tr("A: Apply proposed correction") + itend
//							+ item + tr("&rarr;: Go to next proposal") + itend
//							+ item + tr("&larr;: Go to previous proposal") + itend
//							+ item + tr("C: Switch to clone tool mode") + itend
//							+ item + tr("") + itend
//							+ item + cmd + tr("") + itend
//							+ item + tr("") + itend
//							+ item + cmd + tr("") + itend
						+ QString("</ul>\n")
							);
	repaint();// to force display of splash

	g_splash->show();
	g_splash->raise(); // for full screen mode
	g_splash->update();
}


void EmaMainWindow::on_groupBox_7_clicked()
{

}

void EmaMainWindow::on_zoomx1Button_clicked()
{

}

void EmaMainWindow::on_zoomx2Button_clicked()
{

}


/***************** FILE EXPLORER *********************/
void EmaMainWindow::on_filesShowCheckBox_stateChanged(int state) {
	if(state == Qt::Checked)
		ui->filesTreeWidget->show();
	else
		ui->filesTreeWidget->hide();


}


void EmaMainWindow::on_filesClearButton_clicked() {
	m_imageList.clear();
	ui->filesTreeWidget->clear();
//	ui->scrollAreaWidgetContents->layout()->removeWidget();
}

void EmaMainWindow::on_filesLoadButton_clicked()
{
	QStringList list =  QFileDialog::getOpenFileNames(this,
					 tr("Open Image"),
					 "", //"/home/cseyve",
					 tr("Images (*.png *.p*m *.xpm *.jp* *.tif* *.bmp *.cr2"
						"*.PNG *.P*M *.XPM *.JP* *.TIF* *.BMP *.CR2)"));
	appendFileList(list);
}

void EmaMainWindow::appendFileList(QStringList list) {
	// Append to list
	QStringList::Iterator it = list.begin();
	QString fileName;
	int nb = list.count();

	while(it != list.end()) {
		fileName = (*it);
		++it;

		m_appendFileList.append( fileName );
	}

	ui->loadProgressBar->setValue(0);
	ui->loadProgressBar->show();

	// load in manager
	emaMngr()->appendFileList(list);

	// start timer to update while processing
	if(!m_timer.isActive()) {
		m_timer.start(500);
	}
}


void EmaMainWindow::on_timer_timeout() {

	int val = emaMngr()->getProgress();

	EMAMW_printf(EMALOG_TRACE, "Timeout => progress = %d", val)

	if(val < 100) {

		// update known files : appended files
		QStringList::Iterator it = m_appendFileList.begin();

		QString fileName;
		int nb = m_appendFileList.count();
		EMAMW_printf(EMALOG_TRACE, "Test if we can append %d files...", nb)

		while(it != m_appendFileList.end()) {
			fileName = (*it);
			++it;
			ui->loadProgressBar->setValue(emaMngr()->getProgress());

			appendThumbImage(fileName);
		}

		ui->loadProgressBar->setValue(emaMngr()->getProgress());
	}

	if(!m_appendFileList.isEmpty()) {
		// next update in 500 ms
		m_timer.start(500);
	}

	if(emaMngr()->getProgress() == 100) {
		ui->loadProgressBar->hide();
		m_timer.stop();
	}

}



void EmaMainWindow::on_collecShowCheckBox_stateChanged(int state) {
	if(state == Qt::Checked)
		ui->collecTreeWidget->show();
	else
		ui->collecTreeWidget->hide();
}





void EmaMainWindow::appendThumbImage(QString fileName) {

	t_image_info_struct * pinfo = emaMngr()->getInfo(fileName);

	if(!pinfo) {
		EMAMW_printf(EMALOG_TRACE, "Image file '%s' is NOT YET  managed", fileName.ascii())
	} else {
		// image is already managed
		EMAMW_printf(EMALOG_TRACE, "Image file '%s' is now managed", fileName.ascii())

		// Append to managed pictures
		m_imageList.append(fileName);

		// And remove it from files being waited for
		m_appendFileList.remove(fileName);

		// And display it
		ThumbImageFrame * newThumb = new ThumbImageFrame(
				//ui->imageScrollArea);
				ui->scrollAreaWidgetContents);
		newThumb->setImageFile(fileName, pinfo->thumbImage);
		ui->scrollAreaWidgetContents->layout()->addWidget(newThumb);

		static int nb_item = 0;
		int rowpos = nb_item / 3, colpos = nb_item % 3;
		nb_item++;
		ThumbImageFrame * newThumb2 = new ThumbImageFrame(
				//ui->imageScrollArea);
				ui->gridScrollAreaWidgetContents);
		newThumb2->setImageFile(fileName, pinfo->thumbImage,
							   (int)roundf(pinfo->score * 5.f/100.f));
/*
		ui->gridScrollAreaWidgetContents->layout()->addWidget((QWidget *)newThumb2,
															  rowpos, colpos, 0);
*/

//		ui->gridScrollAreaWidgetContents->layout()->addWidget(newThumb2);

		QGridLayout * grid = (QGridLayout *)ui->gridScrollAreaWidgetContents->layout();
		grid->addWidget(	newThumb2,
							rowpos, colpos, 0);
		// connect this signal
		connect(newThumb, SIGNAL(signalThumbClicked(QString)), this, SLOT(on_thumbImage_clicked(QString)));
		connect(newThumb, SIGNAL(signalThumbSelected(QString)), this, SLOT(on_thumbImage_selected(QString)));
		connect(newThumb2, SIGNAL(signalThumbClicked(QString)), this, SLOT(on_thumbImage_clicked(QString)));
		connect(newThumb2, SIGNAL(signalThumbSelected(QString)), this, SLOT(on_thumbImage_selected(QString)));
	}
}

void EmaMainWindow::on_thumbImage_selected(QString fileName)
{
	QFileInfo fi(fileName);
	if(fi.exists()) {
		t_image_info_struct * pinfo = emaMngr()->getInfo(fileName);
		if(!pinfo) {
			EMAMW_printf(EMALOG_WARNING, "File '%s' is not managed : reload and process file info\n", fileName.ascii())
			ui->imageInfoWidget->setImageFile(fileName);
		} else {
			EMAMW_printf(EMALOG_DEBUG, "File '%s' is managed : use cache info\n", fileName.ascii())
			ui->imageInfoWidget->setImageInfo(pinfo);
			ui->exifScrollArea->setImageInfo(pinfo);
		}
	}
}




void EmaMainWindow::on_thumbImage_clicked(QString fileName)
{
	QFileInfo fi(fileName);
	if(fi.exists()) {
		ui->globalNavImageWidget->setImageFile(fileName);
		ui->mainImageWidget->setImageFile(fileName);

		t_image_info_struct * pinfo = emaMngr()->getInfo(fileName);
		if(!pinfo) {
			EMAMW_printf(EMALOG_WARNING, "File '%s' is not managed : reload and process file info\n", fileName.ascii())
			ui->imageInfoWidget->setImageFile(fileName);
		} else {
			EMAMW_printf(EMALOG_DEBUG, "File '%s' is managed : use cache info\n", fileName.ascii())
			ui->imageInfoWidget->setImageInfo(pinfo);
			ui->exifScrollArea->setImageInfo(pinfo);
		}
	}
}


void EmaMainWindow::on_globalNavImageWidget_signalZoomOn(int x, int y, int scale) {
	ui->mainImageWidget->zoomOn(x,y,scale);
	ui->stackedWidget->setCurrentIndex(0);
}
















static u32 * grayToBGR32 = NULL;
static u32 * grayToBGR32False = NULL;
static u32 * grayToBGR32Red = NULL;

static void init_grayToBGR32()
{
	if(grayToBGR32) {
		return;
	}

	grayToBGR32 = new u32 [256];
	grayToBGR32False = new u32 [256];
	grayToBGR32Red = new u32 [256];
	for(int c = 0; c<256; c++) {
		int Y = c;
		u32 B = Y;// FIXME
		u32 G = Y;
		u32 R = Y;
		grayToBGR32[c] = grayToBGR32Red[c] =
			grayToBGR32False[c] = (R << 16) | (G<<8) | (B<<0);
	}

	// Add false colors
	grayToBGR32[COLORMARK_CORRECTED] = // GREEN
		grayToBGR32False[COLORMARK_CORRECTED] = (255 << 8);
		//mainImage.setColor(COLORMARK_CORRECTED, qRgb(0,255,0));
	// YELLOW
	grayToBGR32False[COLORMARK_REFUSED] = (255 << 8) | (255 << 16);
				//mainImage.setColor(COLORMARK_REFUSED, qRgb(255,255,0));
	grayToBGR32False[COLORMARK_FAILED] =
			grayToBGR32Red[COLORMARK_FAILED] = (255 << 16);
				//mainImage.setColor(COLORMARK_FAILED, qRgb(255,0,0));
	grayToBGR32False[COLORMARK_CURRENT] = // BLUE
	//		grayToBGR32Red[COLORMARK_CURRENT] =
										(255 << 0);
				//mainImage.setColor(COLORMARK_CURRENT, qRgb(0,0,255));
}

