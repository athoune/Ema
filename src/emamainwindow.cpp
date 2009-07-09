#include "emamainwindow.h"
#include "ui_emamainwindow.h"

#include "thumbimageframe.h"

#include <QFileDialog>

EmaMainWindow::EmaMainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::EmaMainWindow)
{
	ui->setupUi(this);
}

EmaMainWindow::~EmaMainWindow()
{
	delete ui;
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
void EmaMainWindow::on_filesShowButton_toggled(bool on) {
	if(on)
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

			// connect this signal
			connect(newThumb, SIGNAL(signalThumbClicked(QString)), this, SLOT(on_thumbImage_clicked(QString)));
		}
}

void EmaMainWindow::on_thumbImage_clicked(QString fileName) {
	QFileInfo fi(fileName);
	if(fi.exists()) {
		ui->globalNavImageWidget->setImageFile(fileName);
		ui->mainImageWidget->setImageFile(fileName);

		ui->imageInfoWidget->setImageFile(fileName);
	}
}

void EmaMainWindow::on_globalNavImageWidget_signalZoomOn(int x, int y, int scale) {
	ui->mainImageWidget->zoomOn(x,y,scale);
}
