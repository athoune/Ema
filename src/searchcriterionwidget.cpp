#include "searchcriterionwidget.h"
#include "ui_searchcriterionwidget.h"

SearchCriterionWidget::SearchCriterionWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::SearchCriterionWidget)
{
    m_ui->setupUi(this);
}

SearchCriterionWidget::~SearchCriterionWidget()
{
    delete m_ui;
}

void SearchCriterionWidget::changeEvent(QEvent *e)
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
