/*
 *
 * Created by : Kmk
 * At : Sept 4th, 2023
 *
 */

#include <QApplication>
#include <QtWidgets>

#include "mainWindow.hpp"


using namespace std;

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	Window win(1100, 700, "Raspberry Pi 3 Sensors");
	win.show();

	return app.exec();
}
