#include "FuckMeApplication.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FuckMeApplication w;
	w.show();
	return a.exec();
}
