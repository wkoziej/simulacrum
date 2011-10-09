/*
 * Population.cpp
 *
 *  Created on: 2011-10-07
 *      Author: wojtas
 */

#include "Population.h"
#include "World.h"

LoggerPtr Population::logger(Logger::getLogger("population"));


Population::Population() {
	productsStock.assign(World::NO_OF_PRODUCTS, 0.0);
}

Population::Population(const JSONObject &population) {
	//this->field = field;
	productsStock.assign(World::NO_OF_PRODUCTS, 0.0);
	if (population.count(L"creatures") > 0) {
		LOG4CXX_TRACE(logger, "Creating creature");
		creaturesTalentsCount = population.count(L"talentsNo");
		creaturesNeedsCount = population.count(L"needsNo");

		JSONArray creatures = population.at(L"creatures")->AsArray();
		JSONArray::iterator creature = creatures.begin();
		for (; creature != creatures.end(); creature++) {
			JSONObject creatureRepresentation = (*creature)->AsObject();
			add(new Creature(creatureRepresentation));
		}
	}
}

Population::~Population() {
}

float Population::objectiveAvarage() {
	// TODO - nie trzeba liczyć za kazdym razem średniej.
	// Nalezy ją zapisać w cache i przy zmianie wartosci na osobniku przeliczać wartość średnią
	SumObjectiveVisitor *visitor = new SumObjectiveVisitor();
	CreaturesOfPopulationOnFieldVisitor *creatureVisitor =
			new CreaturesOfPopulationOnFieldVisitor(visitor);
	creatureVisitor->visit(this, NULL, -1, -1);
	float objectiveSum = visitor->getValue();
	delete creatureVisitor;
	delete visitor;
	return objectiveSum / size();

}
float Population::productNeeds(int i) {
	// Ile osobniki potrzebuja danego produktu
	NeededProductVisitor *visitor = new NeededProductVisitor(i);
	CreaturesOfPopulationOnFieldVisitor *creatureVisitor =
			new CreaturesOfPopulationOnFieldVisitor(visitor);
	creatureVisitor->visit(this, NULL, -1, -1);
	float needed = visitor->getValue();
	delete creatureVisitor;
	delete visitor;
	return needed;
}

float Population::resourceNeeds(int i) {
	// Ile osobniki potrzebuja danego surowca
	NeededResourceVisitor *visitor = new NeededResourceVisitor(i);
	CreaturesOfPopulationOnFieldVisitor *creatureVisitor =
			new CreaturesOfPopulationOnFieldVisitor(visitor);
	creatureVisitor->visit(this, NULL, -1, -1);
	float needed = visitor->getValue();
	// Jaka ilosc produktu jest dostepna w obszarze
	delete creatureVisitor;
	delete visitor;
	return needed;
}

CreaturesOfPopulationOnFieldVisitor::CreaturesOfPopulationOnFieldVisitor(
		CreaturesOnFieldVisitor *v) :
	visitor(v) {
}

void CreaturesOfPopulationOnFieldVisitor::visit(Population *population,
		Field *field, unsigned x, unsigned y) {
	LOG4CXX_DEBUG(logger, "population:" << population << ", Population size: " << population->size());
	int popSize = population->size();
	for (int i = 0; i < popSize; i++) {
		Creature * c = (Creature *) &(population->individual(i,
				GAPopulation::RAW));
		visitor->visit(c, field, population, x, y);
	}
}

LoggerPtr CreaturesOfPopulationOnFieldVisitor::logger(Logger::getLogger(
		"CreaturesVisitor"));

