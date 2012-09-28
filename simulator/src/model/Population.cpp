/*
 * Population.cpp
 *
 *  Created on: 2011-10-07
 *      Author: wojtas
 */

#include "Population.h"
#include "World.h"
#include "ModelHelpers.h"

LoggerPtr CreaturesPopulation::logger(Logger::getLogger("population"));

CreaturesPopulation::CreaturesPopulation(const CreaturesPopulation *species) :
		GAPopulation() {
	//	productsStock.assign(World::NO_OF_ARTICLES, 0.0);
	name = species->getName();
	activitiesCount = species->activitiesCount;
	ga = new GAIncrementalGA(*this);
}

CreaturesPopulation::CreaturesPopulation(Field *field,
		const JSONObject &population, unsigned x, unsigned y) {
	activitiesCount = population.at(L"activitiesCount")->AsNumber();
	name = population.at(L"name")->AsString();
	if (population.count(L"creatures") > 0) {
		LOG4CXX_TRACE(logger, "Creating creature");
		LOG4CXX_DEBUG(logger, "activitiesCount = " << activitiesCount);
		JSONArray creatures = population.at(L"creatures")->AsArray();
		JSONArray::iterator creature = creatures.begin();
		for (; creature != creatures.end(); creature++) {
			JSONObject creatureRepresentation = (*creature)->AsObject();
			add(new Creature(this, field, creatureRepresentation, x, y));
		}
	}
	if (population.count(L"generateRandomCreatures") > 0) {
		int randomCreaturesCount =
				population.at(L"generateRandomCreatures")->AsNumber();
		for (int i = 0; i < randomCreaturesCount; i++) {
			add(new Creature(this, field, x, y));
		}
	}

	ga = new GAIncrementalGA(*this);
	ga->initialize(time(0));

	///////////////
	/*
	 ga = new GASimpleGA (p);

	 ga->initialize(time(0));
	 ga->crossover(GARealTwoPointCrossover);
	 ga->pMutation(MUTATION);
	 ga->pCrossover(CROSSOVER);

	 GASimpleGA simpleGA(*population);
	 simpleGA.initialize(time(0));
	 simpleGA.crossover(GARealTwoPointCrossover);
	 simpleGA.pMutation(MUTATION);
	 simpleGA.pCrossover(CROSSOVER);*/
}



void CreaturesPopulation::reproduce() {
	ga->step();
}

CreaturesPopulation::~CreaturesPopulation() {
}

unsigned CreaturesPopulation::getCreatureActivitiesCount() const {
	return activitiesCount;
}

CreaturesOfPopulationOnFieldVisitor::CreaturesOfPopulationOnFieldVisitor(
		CreaturesOnFieldVisitor *v) :
		visitor(v) {
}

void CreaturesOfPopulationOnFieldVisitor::visit(CreaturesPopulation *population,
		Field *field, unsigned x, unsigned y) {
	LOG4CXX_DEBUG(logger,
			"population:" << population << ", Population size: " << population->size());
	int popSize = population->size();
	for (int i = 0; i < popSize; i++) {
		Creature * c = (Creature *) &(population->individual(i,
				GAPopulation::RAW));
		visitor->visit(c, field, population, x, y);
	}
}

LoggerPtr CreaturesOfPopulationOnFieldVisitor::logger(
		Logger::getLogger("CreaturesVisitor"));

