/*
 * Viewer.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#include <OGRE/OgreString.h>

using namespace Ogre;

#include "Viewer.h"

LoggerPtr Viewer::logger(Logger::getLogger("viewer"));

Viewer::Viewer(World *world) :
	ExampleApplication() {
	this->world = world;
}

/** Configures the application - returns false if the user chooses to abandon configuration. */
bool Viewer::configure(void) {
	if (!mRoot->restoreConfig()) {
		// Show the configuration dialog and initialise the system
		// You can skip this and use root.restoreConfig() to load configuration
		// settings if you were sure there are valid ones saved in ogre.cfg
		if (mRoot->showConfigDialog()) {
			// If returned true, user clicked OK so initialise
			// Here we choose to let the system create a default rendering window by passing 'true'
			mWindow = mRoot->initialise(true);
			return true;
		} else {
			return false;
		}
	} else {
		mWindow = mRoot->initialise(true);
	}
	return true;
}

void Viewer::createFrameListener(void) {

	mFrameListener = new WorldFrameListener(world, mWindow, mCamera);
	mFrameListener->showDebugOverlay(true);
	mRoot->addFrameListener(mFrameListener);
}

void Viewer::createScene(void) {
}

Viewer::~Viewer() {
}
