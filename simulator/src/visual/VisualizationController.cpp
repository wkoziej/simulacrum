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
	LOG4CXX_DEBUG(logger, "assignAvatarToNewCreature " << creatureId.toStdString());
	Avatar *avatar = gameLogic->createCreatureAvatar(creatureId);
	assert (avatar);
	gameLogic->assureAvatarOnField(avatar, x, y);
	gameLogic->animateAvatarBorn(avatar);
}

void VisualizationController::releaseAvatar(QString creatureId, unsigned x,
		unsigned y) {
	Avatar *avatar = gameLogic->findCreatureAvatar(creatureId);
	assert (avatar);
	gameLogic->animateAvatarRelease(avatar);
	delete avatar; // ??
}

void VisualizationController::changeCreaturePosition(QString creatureId,
		unsigned xFrom, unsigned yFrom, unsigned xTo, unsigned yTo) {
	LOG4CXX_TRACE(logger, "changeCreaturePosition");
	Avatar *avatar = gameLogic->findCreatureAvatar(creatureId);
	assert (avatar);
	gameLogic->animateAvatarMovementToField(avatar, xTo, yTo);
}

void VisualizationController::feedCreature(QString creatureId,
		unsigned articleId) {
	LOG4CXX_TRACE(logger, "feedCreature");
	Avatar *avatar = gameLogic->findCreatureAvatar(creatureId);
	gameLogic->animateAvatarEating(avatar, articleId);
}
