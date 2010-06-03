#ifndef FILTERMANAGERFORM_H
#define FILTERMANAGERFORM_H

#include <QWidget>
#include <QResizeEvent>

namespace Ui {
    class FilterManagerForm;
}

class FilterManagerForm : public QWidget {
    Q_OBJECT
public:
    FilterManagerForm(QWidget *parent = 0);
    ~FilterManagerForm();

protected:
    void changeEvent(QEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

private:
    Ui::FilterManagerForm *ui;

signals:
	void signal_resizeEvent(QResizeEvent *e);
};

#endif // FILTERMANAGERFORM_H
