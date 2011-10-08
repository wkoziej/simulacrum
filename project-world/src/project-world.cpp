//============================================================================
// Name        : project-world.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"

#include "model/World.h"
#include "Viewer.h"

using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

int main(int argc, char **argv) {

	if (argc > 1) {
		// BasicConfigurator replaced with PropertyConfigurator.
		PropertyConfigurator::configure(argv[1]);
	} else {
		BasicConfigurator::configure();
	}

	int i = 1298309352;//time(0); //time(0);
	srand(i); //1298309352
	World *world = World::readWorldFromFile("world.json");
	if (world == NULL)
		return 0;
	else {
		Viewer viewer(world);
		//world->step();
		viewer.go();

		cerr << i << endl;
	}
	return 0;
}
