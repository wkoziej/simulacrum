/*
 * MissionFrameListener.h
 *
 *  Created on: 2008-07-15
 *      Author: wkoziej
 */

#ifndef WorldFrameListener_H
#define WorldFrameListener_H

#include <OGRE/Ogre.h>
#include <OGRE/ExampleFrameListener.h>
#include <log4cxx/logger.h>
#include "model/World.h"

using namespace Ogre;

class WorldFrameListener: public ExampleFrameListener {
public:
	WorldFrameListener (World *world, RenderWindow* win, Camera* cam);
	bool frameStarted(const FrameEvent& evt);
private:
	World *world;
	static LoggerPtr logger;
};

#endif /* WorldFrameListener_H */
