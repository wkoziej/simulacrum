/*
 * Viewer.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#ifndef VIEWER_H_
#define VIEWER_H_


#include <CEGUI/CEGUIRenderer.h>
#include <OGRE/Ogre.h>
#include <OGRE/ExampleApplication.h>
#include <log4cxx/logger.h>

#include "WorldFrameListener.h"
#include "model/World.h"

using namespace Ogre;
using namespace CEGUI;
class Viewer : public ExampleApplication  {
public:
	Viewer(World *world);
	virtual bool configure(void);
	virtual void createScene(void);
	virtual void createFrameListener(void);
	virtual ~Viewer();
private:
	static LoggerPtr logger;
	World *world;
};

#endif /* VIEWER_H_ */
