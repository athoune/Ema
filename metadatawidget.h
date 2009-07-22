#ifndef METADATAWIDGET_H
#define METADATAWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
    class MetadataWidget;
}

class MetadataWidget : public QWidget {
    Q_OBJECT
public:
    MetadataWidget(QWidget *parent = 0);
    ~MetadataWidget();
	void setImageFile(const QString & str);
private:
	QString m_fileName;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MetadataWidget *m_ui;
};

#endif // METADATAWIDGET_H
