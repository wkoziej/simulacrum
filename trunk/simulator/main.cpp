#include <iostream>
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"

using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;
/*
 int main(int argc, char *argv[]) {


 if (world == NULL)
 return 0;
 else {
 //Viewer viewer(world);
 StateSaver stateSaver(world, "project-world.db");
 stateSaver.startSession();
 // Zapisz stan świata
 stateSaver.save("start");
 while (true) {
 world->step(&stateSaver);
 // Odnów zasoby świata
 world->nextYear();
 stateSaver.save("nexYear");
 char i;
 //	cin >> i;
 }

 }
 return 0;*/

//#include <QtGui/QApplication>
//#include <Application>
#include <Application.h>
#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>

#include "visual/VisualizationController.h"
#include "visual/SimulatorGameLogic.h"


int main(int argc, char *argv[]) {

	QtOgre::Application app(argc, argv,
			VisualizationController::getPtr()->getGameLogicPtr());

	if (argc > 1) {
		PropertyConfigurator::configure(argv[1]);
	} else {
		PropertyConfigurator::configure("log.properties");
	}
	srand(time(0));


	return app.exec(QtOgre::SuppressSettingsDialog);

}

