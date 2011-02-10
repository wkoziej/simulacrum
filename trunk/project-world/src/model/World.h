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
#include <ga/GASimpleGA.h>

#include "Field.h"
#include "Creature.h"

using namespace std;
using namespace log4cxx;

typedef std::vector<Field *> FieldsVector;
typedef std::vector<FieldsVector> FieldsMatrix;
typedef std::list<Creature *> CreaturesList;
typedef std::vector<GAPopulation *> PopulationsVector;
typedef std::vector<PopulationsVector> PopulationsMatrix;

class CreaturesOnFieldVisitor {
public:
	CreaturesOnFieldVisitor() {
	}
	;
	virtual void visit(Creature *creature, Field *field, GAPopulation *,
			unsigned x, unsigned y) = 0;
	virtual ~CreaturesOnFieldVisitor() {
	}
	;
};

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
	CreaturesList getCreaturesList(const GAPopulation *population) {
		CreaturesList list;
		for (int i = 0; i < population->size(); i++) {
			GAGenome genome = population->individual(i, GAPopulation::RAW);
			list.push_back((Creature *) genome.userData());
		}
		return list;
	}
};

class World {
public:
	World(unsigned X, unsigned Y);
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
private:
	FieldsMatrix fields;
	//CreaturesList creatures;
	PopulationsMatrix populations;
	// Logowanie
	static LoggerPtr logger;

	void iterateCreaturesOnFields(CreaturesOnFieldVisitor *visitor);
	void iteratePopulationOnFields(PopulationOnFieldVisitor *visitor);
};

#endif /* WORLD_H_ */
