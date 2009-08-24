#include "thumbimagewidget.h"
#include "ui_thumbimagewidget.h"

ThumbImageWidget::ThumbImageWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ThumbImageWidget)
{
    m_ui->setupUi(this);
}

ThumbImageWidget::~ThumbImageWidget()
{
    delete m_ui;
}

void ThumbImageWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// Forward mouse events
void ThumbImageWidget::mouseDoubleClickEvent ( QMouseEvent * event ) {
	// forward double click
	emit signal_mouseDoubleClickEvent ( event );

	// signal that we selected one item
//	emit signal_mouseDoubleClickFile ( m_filename );
}

void ThumbImageWidget::mouseMoveEvent ( QMouseEvent * event ) {
	emit signal_mouseMoveEvent ( event );
}
void ThumbImageWidget::mousePressEvent ( QMouseEvent * event ) {
	emit signal_mousePressEvent ( event );
}
void ThumbImageWidget::mouseReleaseEvent ( QMouseEvent * event ) {
	emit signal_mouseReleaseEvent ( event );
}
