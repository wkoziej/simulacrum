/*
 * MissionFrameListener.cpp
 *
 *  Created on: 2008-07-15
 *      Author: wkoziej
 */
#include "Viewer.h"
#include "WorldFrameListener.h"

LoggerPtr WorldFrameListener::logger(Logger::getLogger("frameListener"));

WorldFrameListener::WorldFrameListener(Viewer *v, RenderWindow* win,
		Camera* cam) :
	viewer(v), ExampleFrameListener(win, cam) {
	stepNo = 0;
	autoStep = false;
}

bool WorldFrameListener::frameStarted(const FrameEvent& evt) {
	LOG4CXX_TRACE(logger, "frameStarted");

	if (autoStep) {
		stepAndVisualize();
	}

	return true;
}

void WorldFrameListener::stepAndVisualize() {
	//viewer->world->step();
	switch (stepNo) {
	case 0:
		viewer->world->stepWorld();
		LOG4CXX_INFO(logger, "Renovating")
		;
		break;
	case 1:
		viewer->world->creaturesSupplying();
		LOG4CXX_INFO(logger, "creaturesSupplying")
		;
		break;
	case 2:
		viewer->world->creaturesWorking();
		LOG4CXX_INFO(logger, "creaturesWorking")
		;
		break;
	case 3:
		viewer->world->creaturesMoving();
		LOG4CXX_INFO(logger, "creaturesMoving")
		;
		break;
	case 4:
		viewer->world->creaturesReproducting();
		LOG4CXX_INFO(logger, "creaturesReproducting")
		;
		break;
	case 5:
		viewer->world->creaturesDying();
		LOG4CXX_INFO(logger, "creaturesDying")
		;
		break;
	}
	stepNo++;
	stepNo %= 6;
	// Aktualizuj wizualizację.
	viewer->updateVisualization();

}

bool WorldFrameListener::processUnbufferedKeyInput(const FrameEvent& evt) {
	bool b = true;
	if ((b = ExampleFrameListener::processUnbufferedKeyInput(evt))) {
		if (mKeyboard->isKeyDown(OIS::KC_N) && mTimeUntilNextToggle <= 0) {
			stepAndVisualize();
			mTimeUntilNextToggle = 0.1;
		}

		if (mKeyboard->isKeyDown(OIS::KC_1)  && mTimeUntilNextToggle <= 0) {
			autoStep = !autoStep;
			mTimeUntilNextToggle = 0.1;
		}

	}
	return b;
}
