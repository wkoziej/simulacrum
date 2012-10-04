/*
 * Population.h
 *
 *  Created on: 2011-10-07
 *      Author: wojtas
 */

#ifndef POPULATION_H_
#define POPULATION_H_
#include "JSON/JSONValue.h"
#include <ga/GAPopulation.h>
#include <log4cxx/logger.h>
#include <qt4/QtCore/qmutex.h>
#include "GeneticAlgorithm.h"
#include <list>

class Field;
class Creature;

typedef std::vector<Creature *> Creatures;

using namespace log4cxx;

class CreaturesPopulation //: public GAPopulation
{
public:

	CreaturesPopulation(std::wstring name, int activitiesCount,
			Creature *oneCreature);
	CreaturesPopulation(const CreaturesPopulation *species);
	CreaturesPopulation(Field *field, const JSONObject &population, unsigned x,
			unsigned y);

	void reproduce();
	virtual ~CreaturesPopulation();
	std::wstring getName() const {
		return name;
	}
	;
	unsigned getCreatureActivitiesCount() const;
	unsigned  getNoOfGenes () const;
	const int size() const {
		return creatures.size();
	}
	void remove(const Creature *creatureToRemove);
	void add(Creature *creatureToAdd);
	Creatures creatureList() const {
		return creatures;
	}

	enum SelectionStrategy {
		SS_RANDOM, SS_WORST, SS_BEST
	};

protected:
	const Creature *selectCreature(SelectionStrategy) const;
	void sexualCrossover(const Creature *mom, const Creature *dad,
			Creature *sister, Creature *brother) const;
	void onePointCrossover(const Creature *mom, const Creature *dad,
			Creature *sister, Creature *brother) const;
private:
	//Field *field;
	//std::vector<float> productsStock;
	std::wstring name;
	unsigned activitiesCount;
	//int creaturesTalentsCount;
	//int creaturesNeedsCount;
	// Logowanie
	static LoggerPtr logger;
	//GeneticAlgorithm *ga;
	Creatures creatures;
	QMutex polulationChange;
};

typedef std::pair<std::wstring, CreaturesPopulation *> NamedPopulation;
typedef std::map<std::wstring, CreaturesPopulation *> PopulationsMap;

#endif /* POPULATION_H_ */
