#ifndef SEARCHCRITERIONWIDGET_H
#define SEARCHCRITERIONWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
    class SearchCriterionWidget;
}

class SearchCriterionWidget : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(SearchCriterionWidget)
public:
    explicit SearchCriterionWidget(QWidget *parent = 0);
    virtual ~SearchCriterionWidget();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::SearchCriterionWidget *m_ui;
};

#endif // SEARCHCRITERIONWIDGET_H
