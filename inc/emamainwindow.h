#ifndef EmaMAINWINDOW_H
#define EmaMAINWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui
{
	class EmaMainWindow;
}

class EmaMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	EmaMainWindow(QWidget *parent = 0);
	~EmaMainWindow();

private:
	Ui::EmaMainWindow *ui;

	/// Opened image paths
	QStringList m_imageList;

	void appendThumbImage(QString fileName);
public slots:
	void on_thumbImage_clicked(QString fileName);

private slots:
	void on_filesShowCheckBox_stateChanged(int);
	void on_filesLoadButton_clicked();
	void on_filesClearButton_clicked();
	void on_zoomx2Button_clicked();
	void on_zoomx1Button_clicked();
	void on_groupBox_7_clicked();

	// Collections
	void on_collecShowCheckBox_stateChanged(int);



	void on_gridButton_clicked();
	void on_imgButton_clicked();

	//
	void on_globalNavImageWidget_signalZoomOn(int, int, int);

};

#endif // EmaMAINWINDOW_H
