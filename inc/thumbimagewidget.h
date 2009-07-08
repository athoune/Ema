#ifndef THUMBIMAGEWIDGET_H
#define THUMBIMAGEWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
    class ThumbImageWidget;
}

class ThumbImageWidget : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(ThumbImageWidget)
public:
    explicit ThumbImageWidget(QWidget *parent = 0);
    virtual ~ThumbImageWidget();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ThumbImageWidget *m_ui;
};

#endif // THUMBIMAGEWIDGET_H
