/*
 * World.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
#include <list>
#include <log4cxx/logger.h>


#include "Field.h"
#include "Creature.h"
#include "Population.h"
using namespace std;
using namespace log4cxx;

typedef std::vector<Field *> FieldsVector;
typedef std::vector<FieldsVector> FieldsMatrix;
typedef std::list<Creature *> CreaturesList;
typedef std::vector<Population *> PopulationsVector;
typedef std::vector<PopulationsVector> PopulationsMatrix;

class PopulationOnFieldVisitor {
public:
	PopulationOnFieldVisitor() {
	}
	;
	virtual void visit(GAPopulation *population, Field *field, unsigned x,
			unsigned y) = 0;
	virtual ~PopulationOnFieldVisitor() {
	}
	;
protected:
};

class World {
public:
	static World *createWorld(unsigned X, unsigned Y);
	static World *getWorld() { return singleton; };
	virtual ~World();
	void initializeRandomly();
	void createCreatures();
	void creaturesWorking();
	void creaturesSupplying();
	void creaturesDying();
	void creaturesReproducting();
	void creaturesMoving();
	void resourcesRenovation();
	bool creaturesExists();
	void step();
	FieldsMatrix fields;
	PopulationsMatrix populations;
	void iteratePopulationOnFields(PopulationOnFieldVisitor *visitor);
private:
	static World *singleton;
	World() {
		if (singleton == NULL)
			singleton = this;
	}
	//CreaturesList creatures;

	// Logowanie
	static LoggerPtr logger;

	void iterateCreaturesOnFields(CreaturesOnFieldVisitor *visitor);

};

#endif /* WORLD_H_ */
