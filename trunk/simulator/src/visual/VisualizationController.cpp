/*
 * VisualizationControler.cpp
 *
 *  Created on: 23-10-2011
 *      Author: wojtas
 */

#include "VisualizationController.h"

#include <OGRE/OgreSceneManager.h>
LoggerPtr VisualizationController::logger(Logger::getLogger(
		"visualizationController"));

VisualizationController *VisualizationController::singleton;
VisualizationController::VisualizationController() {
	if (singleton == NULL) {
		singleton = this;
		gameLogic = new SimulatorGameLogic();
	}
}

VisualizationController::~VisualizationController() {
}

void VisualizationController::assignAvatarToNewCreature(QString creatureId,
		unsigned x, unsigned y) {
	LOG4CXX_TRACE(logger, "assignAvatarToNewCreature");
	Avatar *avatar = gameLogic->createCreatureAvatar(creatureId);
	gameLogic->assureAvatarOnField(avatar, x, y);
	gameLogic->animateAvatarBorn(avatar);
}

void VisualizationController::changeCreaturePosition(QString creatureId,
		unsigned xFrom, unsigned yFrom, unsigned xTo, unsigned yTo) {
	LOG4CXX_TRACE(logger, "changeCreaturePosition");
	Avatar *avatar = gameLogic->findCreatureAvatar(creatureId);
	gameLogic->animateAvatarMovementToField(avatar, xTo, yTo);
	/*
	 if (mSceneManager) {
	 mSceneManager->getSceneNode("CubeNode2")->setScale(
	 randBetweenAndStepped(0.1, 1.0, 0.1), randBetweenAndStepped(
	 0.1, 1.0, 0.1), randBetweenAndStepped(0.1, 1.0, 0.1));
	 }
	 */
}

void VisualizationController::feedCreature(QString creatureId,
		unsigned articleId) {
	LOG4CXX_TRACE(logger, "feedCreature");
	Avatar *avatar = gameLogic->findCreatureAvatar(creatureId);
	gameLogic->animateAvatarEating(avatar, articleId);
	/*
	 if (mSceneManager) {
	 mSceneManager->getSceneNode("CubeNode1")->setScale(
	 randBetweenAndStepped(0.1, 1.0, 0.1), randBetweenAndStepped(
	 0.1, 1.0, 0.1), randBetweenAndStepped(0.1, 1.0, 0.1));
	 }*/
}
