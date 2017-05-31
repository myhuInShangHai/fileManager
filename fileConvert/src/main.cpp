#include "convertWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	ConvertWidget widget;
	widget.show();
	return app.exec();


}