#ifndef EXIFDISPLAYSCROLLAREA_H
#define EXIFDISPLAYSCROLLAREA_H

#include <QtGui/QScrollArea>

namespace Ui {
    class ExifDisplayScrollArea;
}

class ExifDisplayScrollArea : public QScrollArea {
    Q_OBJECT
    Q_DISABLE_COPY(ExifDisplayScrollArea)
public:
    explicit ExifDisplayScrollArea(QWidget *parent = 0);
    virtual ~ExifDisplayScrollArea();
	void setImageFile(const QString & str);
protected:
    virtual void changeEvent(QEvent *e);

private:
	QString m_fileName;
    Ui::ExifDisplayScrollArea *m_ui;
};

#endif // EXIFDISPLAYSCROLLAREA_H
