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
