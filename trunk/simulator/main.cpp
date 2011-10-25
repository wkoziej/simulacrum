#include <iostream>
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "model/World.h"
#include "StateSaver.h"
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
#include <QtCore/QThread>

#include "visual/VisualizationController.h"
#include "visual/SimulatorGameLogic.h"

class WorldThred: public QThread {
	StateSaver *stateSaver;
public:
	WorldThred(StateSaver *stateSaver) {
		this->stateSaver = stateSaver;
	}
	void run() {
		stateSaver->save("start");
		while (true) {
			World::getPtr()->step(stateSaver);
			// Odnów zasoby świata
			World::getPtr()->nextYear();
			stateSaver->save("nexYear");
		}

	}
};

int main(int argc, char *argv[]) {

	QtOgre::Application app(argc, argv,
			VisualizationController::getPtr()->getGameLogicPtr());

	if (argc > 1) {
		PropertyConfigurator::configure(argv[1]);
	} else {
		PropertyConfigurator::configure("log.properties");
	}
	srand(time(0));

	World *world = World::readWorldFromFile("world.json");
	StateSaver stateSaver(world, "project-world.db");

	WorldThred *worldThred = new WorldThred(&stateSaver);
	worldThred->start();

	return app.exec(QtOgre::SuppressSettingsDialog);

}

