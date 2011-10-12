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


class Viewer;

using namespace Ogre;
using namespace log4cxx;

class WorldFrameListener: public ExampleFrameListener {
public:
	WorldFrameListener (Viewer *v, RenderWindow* win, Camera* cam);
	bool frameStarted(const FrameEvent& evt);
	bool processUnbufferedKeyInput(const FrameEvent& evt);

private:
	static LoggerPtr logger;
	Viewer *viewer;
	int stepNo;
	bool autoStep;
    void stepAndVisualize();
};

#endif /* WorldFrameListener_H */
