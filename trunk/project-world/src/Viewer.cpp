/*
 * Viewer.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#include <OGRE/OgreString.h>

using namespace Ogre;

#include "Viewer.h"

class SceneCreatorVisitor: public PopulationOnFieldVisitor {

private:
	Viewer *viewer;
public:
	SceneCreatorVisitor(Viewer *v) :
		viewer(v) {

	}

	void createFieldVisualization(SceneNode *&mNode, Field *field, unsigned x,
			unsigned y) {
		Entity *mEntity;
		Ogre::String name = "field " + Ogre::StringConverter::toString(x) + " "
				+ Ogre::StringConverter::toString(y);
		mEntity = viewer->mSceneMgr->createEntity(name,
				Ogre::SceneManager::PT_PLANE);
		mNode = viewer->mSceneMgr->getRootSceneNode()->createChildSceneNode(
				"child_" + name, Ogre::Vector3::ZERO);
		mEntity->setMaterialName("field");
		// mNode->attachObject(mEntity);
		mNode->setPosition(x * 100, y * 100, 0);
		mNode->setScale(0.2, 0.2, 0.2);
		mNode->getUserObjectBindings().setUserAny(Ogre::Any(field));
	}

	void createResourceVisualization(SceneNode *fieldNode, Field *field,
			unsigned x, unsigned y, std::vector<SceneNode *> &resourceNodes) {
		FloatVector::const_iterator r =
				field->resourceQuantitiesVector().begin();
		unsigned rIndex = 0;
		for (; r != field->resourceQuantitiesVector().end(); r++, rIndex++) {
			SceneNode *mNode;
			Entity *mEntity;
			Ogre::String name = "res_"
					+ Ogre::StringConverter::toString(rIndex) + "_on_field_"
					+ Ogre::StringConverter::toString(x) + " "
					+ Ogre::StringConverter::toString(y);
			mEntity = viewer->mSceneMgr->createEntity(name,
					Ogre::SceneManager::PT_CUBE);
			mNode = fieldNode->createChildSceneNode("res_child_" + name,
					Ogre::Vector3::ZERO);
			mEntity->setMaterialName("resource");
			mNode->attachObject(mEntity);
			//Real x = rIndex * (100 / field->resourceQuantitiesVector().size());
			//Real y = 50;
			Real z = 15;
			mNode->setPosition(x, y, z);
			mNode->setScale(0.3, 0.3, *r);
			resourceNodes.push_back(mNode);
		}
	}

	void createProductVisualization(SceneNode *fieldNode, Field *field,
			unsigned x, unsigned y, std::vector<SceneNode *> &productNodes) {
		/*FloatVector::const_iterator r =
				field->productsQuantitiesVector().begin();
		unsigned rIndex = 0;
		for (; r != field->productsQuantitiesVector().end(); r++, rIndex++) {
			SceneNode *mNode;
			Entity *mEntity;
			Ogre::String name = "prod_" + Ogre::StringConverter::toString(
					rIndex) + "_on_field_" + Ogre::StringConverter::toString(x)
					+ " " + Ogre::StringConverter::toString(y);
			mEntity = viewer->mSceneMgr->createEntity(name,
					Ogre::SceneManager::PT_CUBE);
			mNode = fieldNode->createChildSceneNode("prod_child_" + name,
					Ogre::Vector3::ZERO);
			mEntity->setMaterialName("product");
			mNode->attachObject(mEntity);
			mNode->setPosition(rIndex * (100
					/ field->productsQuantitiesVector().size()), 0, 15);
			mNode->setScale(0.3, 0.3, *r);
			productNodes.push_back(mNode);
		}*/
	}

	void createPopulationVisualization(SceneNode *fieldNode,
			GAPopulation *population, unsigned x, unsigned y, SceneNode *&mNode) {
		Entity *mEntity;
		Ogre::String name = "pop_on_field_"
				+ Ogre::StringConverter::toString(x) + " "
				+ Ogre::StringConverter::toString(y);
		mEntity = viewer->mSceneMgr->createEntity(name,
				Ogre::SceneManager::PT_CUBE);
		mNode = fieldNode->createChildSceneNode("pop_child_" + name,
				Ogre::Vector3::ZERO);
		mEntity->setMaterialName("population");
		mNode->attachObject(mEntity);
		mNode->setPosition(0, -50, 15);
		mNode->setScale(0.6, 0.3, population->size() / 10.0);
		//mNode->setUserAny(Ogre::Any(population));
		mNode->getUserObjectBindings().setUserAny(Ogre::Any(population));
		//UserObjectBindings::
	}

	virtual void visit(GAPopulation *population, Field *field, unsigned x,
			unsigned y) {
		// Płaszczyzny pola
		//SceneNode *fieldNode;
		FieldVisualData fVisualData;
		createFieldVisualization(fVisualData.fieldNode, field, x, y);
		// Zasoby
		createResourceVisualization(fVisualData.fieldNode, field, x, y,
				fVisualData.resourceNodes);
		// Produkty
		//createProductVisualization(fVisualData.fieldNode, field, x, y,
		//		fVisualData.productNodes);
		// Populacja
		//createPopulationVisualization(fVisualData.fieldNode, population, x, y,
		//		fVisualData.populationNode);
		viewer->fVisualData.push_back(fVisualData);
	}
};

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

	mFrameListener = new WorldFrameListener(this, mWindow, mCamera);
	mFrameListener->showDebugOverlay(true);
	mRoot->addFrameListener(mFrameListener);
}

void Viewer::createScene(void) {
	// Na podstaiwe pól świata tworzymy matrycę MxN pól które będziemy wizualizować.
	// Kazde pole składa się z listy surowców, listy produktów oraz osobników na tym polu
	SceneCreatorVisitor *visitor = new SceneCreatorVisitor(this);
	world->iteratePopulationOnFields(visitor);
	delete visitor;
}

void Viewer::updateResourceVisualization(
		std::vector<FieldVisualData>::iterator & i) {
	Ogre::Any any;
	Ogre::Vector3 s;
	any = i->fieldNode->getUserObjectBindings().getUserAny();
	Field *f = any_cast<Field*> (any);
	std::vector<SceneNode*>::iterator r = i->resourceNodes.begin();
	FloatVector::const_iterator rV = f->resourceQuantitiesVector().begin();
	for (; r != i->resourceNodes.end(); r++, rV++) {
		s = (*r)->getScale();
		(*r)->setScale(Ogre::Vector3::UNIT_SCALE);
		(*r)->setScale(s.x, s.y, *rV / 10.0);
		if (*rV == 0) {
			//(*r)->setScale(0.001, 0.001, 0.001);
		}
		LOG4CXX_INFO(logger, "\t resource [" << r - i->resourceNodes.begin() << "]: " << *rV);
	}
}

void Viewer::updateProductVisualization(
		std::vector<FieldVisualData>::iterator & i) {
	/*Ogre::Any any;
	Ogre::Vector3 s;
	any = i->fieldNode->getUserObjectBindings().getUserAny();
	Field *f = any_cast<Field*> (any);
	std::vector<SceneNode*>::iterator r = i->productNodes.begin();
	FloatVector::const_iterator rV = f->productsQuantitiesVector().begin();
	for (; r != i->productNodes.end(); r++, rV++) {
		s = (*r)->getScale();
		(*r)->setScale(Ogre::Vector3::UNIT_SCALE);
		(*r)->setScale(s.x, s.y, *rV / 10.0);
		if (*rV == 0) {
			//(*r)->setScale(0.001, 0.001, 0.001);
		}
		LOG4CXX_INFO(logger, "\t product [" << r - i->productNodes.begin() << "]: " << *rV);
	}*/
}

void Viewer::updatePopulationVisualization(
		std::vector<FieldVisualData>::iterator i) {
	Ogre::Vector3 s;
	Ogre::Any any;
	any = i->populationNode->getUserObjectBindings().getUserAny();
	GAPopulation *p = any_cast<GAPopulation*> (any);
	s = i->populationNode->getScale();
	LOG4CXX_DEBUG(logger, "population scale:" << s);
	int popSize = p->size();
	if (popSize) {
		s.z = popSize / 10.0;
	} else {
		s.z = 0;
	}
	i->populationNode->setScale(Ogre::Vector3::UNIT_SCALE);
	i->populationNode->setScale(s.x, s.y, s.z);
	if (popSize == 0) {
		//i->populationNode->setScale(0.001, 0.001, 0.001);
	}
	LOG4CXX_INFO(logger, "\t population: " << popSize);
}

void Viewer::updateVisualization() {
	std::vector<FieldVisualData>::iterator i = fVisualData.begin();
	LOG4CXX_INFO(logger, " - - -------------------------------- - - ");
	for (; i != fVisualData.end(); i++) {
		LOG4CXX_INFO(logger, "FIELD ["<< i - fVisualData.begin() << "] ");
		updateResourceVisualization(i);
		updateProductVisualization(i);
		updatePopulationVisualization(i);
	}
}

Viewer::~Viewer() {
}
