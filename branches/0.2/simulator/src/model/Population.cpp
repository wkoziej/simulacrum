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
class UpdateValueVisitor: public CreaturesOnFieldVisitor {
protected:
	float value;
private:

	int productIndex;
public:
	UpdateValueVisitor() :
		value(0.0) {
	}

	float getValue() {
		return value;
	}

	void visit(Creature *creature, Field *field, CreaturesPopulation *p,
			unsigned x, unsigned y) {
		updateValue(creature, field, p, x, y);
	}

	virtual void updateValue(Creature *creature, Field *field,
			CreaturesPopulation *p, unsigned x, unsigned y) = 0;
};

class NeededProductVisitor: public UpdateValueVisitor {
private:
	int index;
public:
	NeededProductVisitor(int productIndex) {
		this->index = productIndex;
	}

	void updateValue(Creature *creature, Field *field, CreaturesPopulation *p,
			unsigned x, unsigned y) {
		value += creature->getNeedOfProductRatio(index);
	}
};

class NeededResourceVisitor: public UpdateValueVisitor {
private:
	int index;
public:
	NeededResourceVisitor(int productIndex) {
		this->index = productIndex;
	}

	void updateValue(Creature *creature, Field *field, CreaturesPopulation *p,
			unsigned x, unsigned y) {
		value += creature->getNeedOfResource(index);
	}
};

class SumObjectiveVisitor: public UpdateValueVisitor {
public:
	void updateValue(Creature *creature, Field *field, CreaturesPopulation *p,
			unsigned x, unsigned y) {
		value += creature->score();
	}
};

CreaturesPopulation::CreaturesPopulation() {
	productsStock.assign(World::NO_OF_PRODUCTS, 0.0);
	name = L"noname";
	creaturesTalentsCount = 2;
	creaturesNeedsCount = 2;
}

CreaturesPopulation::CreaturesPopulation(const Field *field, const JSONObject &population) {
	//this->field = field;
	productsStock.assign(World::NO_OF_PRODUCTS, 0.0);
	if (population.count(L"creatures") > 0) {
		LOG4CXX_TRACE(logger, "Creating creature");
		creaturesTalentsCount = population.at(L"talentsNo")->AsNumber();
		creaturesNeedsCount = population.at(L"needsNo")->AsNumber();
		name = population.at(L"name")->AsString();
		LOG4CXX_DEBUG(logger, "creaturesTalentsCount = " << creaturesTalentsCount << ", creaturesNeedsCount =  " << creaturesNeedsCount);
		JSONArray creatures = population.at(L"creatures")->AsArray();
		JSONArray::iterator creature = creatures.begin();
		for (; creature != creatures.end(); creature++) {
			JSONObject creatureRepresentation = (*creature)->AsObject();
			add(new Creature(this, field, creatureRepresentation));
		}
	}
}

CreaturesPopulation::~CreaturesPopulation() {
}

float CreaturesPopulation::objectiveAvarage() {
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

float CreaturesPopulation::keptProductSum(int i) {
	return productsStock.at(i);
}

void CreaturesPopulation::updateProductStock(int i, float delta) {
	//if (productsStock.at(i) + delta > 0)
	LOG4CXX_DEBUG(logger, "productsStock.at(" << i << ") = " << productsStock.at(i) << ", delta = " << delta );
	productsStock.at(i) += delta;
	assert (productsStock.at(i) >= 0);
}

float CreaturesPopulation::productNeeds(int i) {
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

float CreaturesPopulation::resourceNeeds(int i) {
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

void CreaturesOfPopulationOnFieldVisitor::visit(CreaturesPopulation *population,
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

