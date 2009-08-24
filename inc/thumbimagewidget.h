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
	virtual void mouseDoubleClickEvent ( QMouseEvent * event );
	virtual void mouseMoveEvent ( QMouseEvent * event );
	virtual void mousePressEvent ( QMouseEvent * event );
	virtual void mouseReleaseEvent ( QMouseEvent * event );
private:
    Ui::ThumbImageWidget *m_ui;

signals:
	void signal_mouseDoubleClickFile ( QString filename );

	void signal_mouseDoubleClickEvent ( QMouseEvent * event );
	void signal_mouseMoveEvent ( QMouseEvent * event );
	void signal_mousePressEvent ( QMouseEvent * event );
	void signal_mouseReleaseEvent ( QMouseEvent * event );
};

#endif // THUMBIMAGEWIDGET_H
