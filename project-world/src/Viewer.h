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

class SceneCreatorVisitor;

struct FieldVisualData {
	Field *field;
	SceneNode * fieldNode;
	std::vector<SceneNode *> resourceNodes;
	std::vector<SceneNode *> productNodes;
	SceneNode * populationNode;
};

class Viewer : public ExampleApplication  {
	friend class SceneCreatorVisitor;
public:
	Viewer(World *world);
	virtual bool configure(void);
	virtual void createScene(void);
	virtual void createFrameListener(void);
	virtual ~Viewer();
	void updateVisualization ();
	World *world;
private:
	static LoggerPtr logger;

	std::vector<FieldVisualData> fVisualData;
    void updateResourceVisualization(std::vector<FieldVisualData>::iterator &i);
    void updateProductVisualization(std::vector<FieldVisualData>::iterator &i);
    void updatePopulationVisualization(std::vector<FieldVisualData>::iterator i);
};

#endif /* VIEWER_H_ */
