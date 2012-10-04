#include <QtGui/QApplication>
#include "mainwindow.h"
#include <log4cxx/propertyconfigurator.h>
using namespace log4cxx;
int main(int argc, char *argv[])
{
	PropertyConfigurator::configure("log.properties");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
