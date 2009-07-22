#include "emamainwindow.h"
#include "ui_emamainwindow.h"

#include "thumbimageframe.h"

#include <QFileDialog>
#include <QSplashScreen>




EmaMainWindow::EmaMainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::EmaMainWindow)
{
	ui->setupUi(this);
}

EmaMainWindow::~EmaMainWindow()
{
	delete ui;
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
	g_splash->showMessage( tr("<b>Ema</b> version: ")
						  + verstr.sprintf("svn%04d%02d%02d", VERSION_YY, VERSION_MM, VERSION_DD)

						  + QString("<br>Website & Wiki: <a href=\"http://tamanoir.googlecode.com/\">http://tamanoir.googlecode.com/</a><br><br>")
						  + tr("Shortcuts :<br><ul>\n")
							+ item + cmd + tr("O: Open a picture file") + itend
							+ item + cmd + tr("S: Save corrected image") + itend
							+ item + cmd + tr("H: Display version information") + itend
							+ item + tr("M: Mark current crop area in red") + itend
							+ item + tr("A: Apply proposed correction") + itend
							+ item + tr("&rarr;: Go to next proposal") + itend
							+ item + tr("&larr;: Go to previous proposal") + itend
							+ item + tr("C: Switch to clone tool mode") + itend
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
	QStringList::Iterator it = list.begin();

	QString fileName;
	while(it != list.end()) {
		fileName = (*it);
		++it;

		appendThumbImage(fileName);
	}

	on_thumbImage_clicked(fileName);
}


void EmaMainWindow::on_collecShowCheckBox_stateChanged(int state) {
	if(state == Qt::Checked)
		ui->collecTreeWidget->show();
	else
		ui->collecTreeWidget->hide();
}





void EmaMainWindow::appendThumbImage(QString fileName) {
	if(!m_imageList.contains(fileName)) {
			// Append to managed pictures
			m_imageList.append(fileName);

			// And display it
			ThumbImageFrame * newThumb = new ThumbImageFrame(
					//ui->imageScrollArea);
					ui->scrollAreaWidgetContents);
			newThumb->setImageFile(fileName);
			ui->scrollAreaWidgetContents->layout()->addWidget(newThumb);

			ThumbImageFrame * newThumb2 = new ThumbImageFrame(
					//ui->imageScrollArea);
					ui->gridScrollAreaWidgetContents);
			newThumb2->setImageFile(fileName);
			ui->gridScrollAreaWidgetContents->layout()->addWidget(newThumb2);

			// connect this signal
			connect(newThumb, SIGNAL(signalThumbClicked(QString)), this, SLOT(on_thumbImage_clicked(QString)));
			connect(newThumb2, SIGNAL(signalThumbClicked(QString)), this, SLOT(on_thumbImage_clicked(QString)));
		}
}

void EmaMainWindow::on_thumbImage_clicked(QString fileName) {
	QFileInfo fi(fileName);
	if(fi.exists()) {
		ui->globalNavImageWidget->setImageFile(fileName);
		ui->mainImageWidget->setImageFile(fileName);

		ui->imageInfoWidget->setImageFile(fileName);

		ui->exifScrollArea->setImageFile(fileName);
	}
}

void EmaMainWindow::on_globalNavImageWidget_signalZoomOn(int x, int y, int scale) {
	ui->mainImageWidget->zoomOn(x,y,scale);
	ui->stackedWidget->setCurrentIndex(0);
}
