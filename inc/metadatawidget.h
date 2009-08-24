#ifndef METADATAWIDGET_H
#define METADATAWIDGET_H

#include <QtGui/QWidget>
#include "imageinfo.h"

namespace Ui {
    class MetadataWidget;
}

class MetadataWidget : public QWidget {
    Q_OBJECT
public:
    MetadataWidget(QWidget *parent = 0);
    ~MetadataWidget();
	void setImageFile(const QString & str);
	void setImageInfo(t_image_info_struct * p_image_info_struct);

private:
	QString m_fileName;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MetadataWidget *m_ui;
};

#endif // METADATAWIDGET_H
