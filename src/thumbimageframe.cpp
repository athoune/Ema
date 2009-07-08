#include "thumbimageframe.h"
#include "ui_thumbimageframe.h"

ThumbImageFrame::ThumbImageFrame(QWidget *parent) :
	QFrame(parent),
	m_ui(new Ui::ThumbImageFrame)
{
	m_ui->setupUi(this);
}

ThumbImageFrame::~ThumbImageFrame()
{
	delete m_ui;
}

void ThumbImageFrame::changeEvent(QEvent *e)
{
	QFrame::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void ThumbImageFrame::setImageFile(const QString & imagePath)
{
	// "NavImageWidget::%s:%d ('%s')\n",
	//		__func__, __LINE__,
	//		imagePath);

	QImage fullImage(imagePath);
	if(fullImage.isNull()) {
		m_displayImage.fill(127);
		m_imagePath = "";
	}
	else {
		int wdisp = m_ui->globalImageLabel->width()-2;
		int hdisp = m_ui->globalImageLabel->height()-2;
		m_imagePath = imagePath;
		m_displayImage = fullImage.scaled( wdisp, hdisp,
								Qt::KeepAspectRatio );

	}
	m_ui->globalImageLabel->setPixmap(QPixmap::fromImage(m_displayImage));
}

void ThumbImageFrame::on_globalImageLabel_signalMousePressEvent(QMouseEvent * e) {
	emit signalThumbClicked(m_imagePath);
}
