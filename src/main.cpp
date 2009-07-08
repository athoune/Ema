#include <QtGui/QApplication>
#include "emamainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	EmaMainWindow w;
	w.show();
	return a.exec();
}
