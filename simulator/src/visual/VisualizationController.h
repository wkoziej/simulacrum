/*
 * VisualizationControler.h
 *
 *  Created on: 23-10-2011
 *      Author: wojtas
 */

#ifndef VISUALIZATIONCONTROLER_H_
#define VISUALIZATIONCONTROLER_H_
#include <QtCore/QObject>
#include <log4cxx/logger.h>
#include "SimulatorGameLogic.h"
using namespace log4cxx;

class VisualizationController: public QObject, public SimulatorGameLogic {
Q_OBJECT
public:
	virtual ~VisualizationController();
public:
	static VisualizationController *getPtr() {
		if (singleton == NULL)
			singleton = new VisualizationController();
		return singleton;
	}

	GameLogic *getGameLogicPtr() {
		return gameLogic;
	}

public slots:
	void assignAvatarToNewCreature(QString creatureId, unsigned x, unsigned y);
	void releaseAvatar (QString creatureId, unsigned x, unsigned y);
	void changeCreaturePosition(QString creatureId, unsigned xFrom,
			unsigned yFrom, unsigned xTo, unsigned yTo);
	void feedCreature(QString creatureId, unsigned articleId);

protected:
	static LoggerPtr logger;
	static VisualizationController *singleton;
	VisualizationController();
	SimulatorGameLogic *gameLogic;

};

#endif /* VISUALIZATIONCONTROLER_H_ */
