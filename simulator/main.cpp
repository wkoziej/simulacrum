#include <iostream>
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "model/World.h"
#include "StateSaver.h"
using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

int main(int argc, char *argv[]) {
	if (argc > 1) {
		// BasicConfigurator replaced with PropertyConfigurator.
		PropertyConfigurator::configure(argv[1]);
	} else {
		//BasicConfigurator::configure();
		PropertyConfigurator::configure("log.properties");
	}

	int i = 1298309352; //time(0); //time(0);
	srand(i); //1298309352
	World *world = World::readWorldFromFile("world.json");

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
			//char i;
			//cin >> i;
		}

	}
	return 0;
}
