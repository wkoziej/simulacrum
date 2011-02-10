/*
 * MissionFrameListener.cpp
 *
 *  Created on: 2008-07-15
 *      Author: wkoziej
 */

#include "WorldFrameListener.h"


LoggerPtr WorldFrameListener::logger(Logger::getLogger("frameListener"));

WorldFrameListener::WorldFrameListener(World *world, RenderWindow* win,
		Camera* cam) :
	ExampleFrameListener(win, cam) {
	this->world = world;
}

bool WorldFrameListener::frameStarted(const FrameEvent& evt) {
	LOG4CXX_TRACE(logger, "frameStarted");
	world->step();
	return true;
}

