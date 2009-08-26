/***************************************************************************
 *  thumbimageframe - Thumb display picture
 *
 *  Jul 2 21:10:56 2009
 *  Copyright  2007  Christophe Seyve
 *  Email cseyve@free.fr
 ****************************************************************************/
#ifndef THUMBIMAGEFRAME_H
#define THUMBIMAGEFRAME_H

#include <QtGui/QFrame>
#include "imageinfo.h"

namespace Ui {
	class ThumbImageFrame;
}

class ThumbImageFrame : public QFrame {
	Q_OBJECT
	Q_DISABLE_COPY(ThumbImageFrame)
public:
	explicit ThumbImageFrame(QWidget *parent = 0);
	virtual ~ThumbImageFrame();
	/** @brief Set the background image */
	void setImageFile(const QString &  imagePath, IplImage * img = NULL, int score = -1);

protected:
	virtual void changeEvent(QEvent *e);

private:
	QString m_imagePath;
	Ui::ThumbImageFrame *m_ui;

private slots:
	void on_globalImageLabel_signalMousePressEvent(QMouseEvent * e);
	void on_globalImageLabel_signalMouseMoveEvent(QMouseEvent * e);
signals:
	void signalThumbClicked(QString);
	void signalThumbSelected(QString);

private:
	QString m_filename;
protected:
	virtual void mouseDoubleClickEvent ( QMouseEvent * event );
	virtual void mouseMoveEvent ( QMouseEvent * event );
	virtual void mousePressEvent ( QMouseEvent * event );
	virtual void mouseReleaseEvent ( QMouseEvent * event );
signals:
//	void changeEvent(QEvent *e);
	void signal_mouseDoubleClickFile ( QString filename );

	void signal_mouseDoubleClickEvent ( QMouseEvent * event );
	void signal_mouseMoveEvent ( QMouseEvent * event );
	void signal_mousePressEvent ( QMouseEvent * event );
	void signal_mouseReleaseEvent ( QMouseEvent * event );
};

#endif // THUMBIMAGEFRAME_H
