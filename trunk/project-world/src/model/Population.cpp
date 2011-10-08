/*
 * Population.cpp
 *
 *  Created on: 2011-10-07
 *      Author: wojtas
 */

#include "Population.h"
#include "World.h"

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

	void visit(Creature *creature, Field *field, Population *p, unsigned x,
			unsigned y) {
		updateValue(creature, field, p, x, y);
		//needed += creature->getNeedOfProduct(productIndex);
	}

	virtual void updateValue(Creature *creature, Field *field, Population *p,
			unsigned x, unsigned y) = 0;
};

class NeededProductVisitor: public UpdateValueVisitor {
private:
	int index;
public:
	NeededProductVisitor(int productIndex) {
		this->index = productIndex;
	}

	void updateValue(Creature *creature, Field *field, Population *p,
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

	void updateValue(Creature *creature, Field *field, Population *p,
			unsigned x, unsigned y) {
		value += creature->getNeedOfResource(index);
	}
};

class SumObjectiveVisitor: public UpdateValueVisitor {
public:
	void updateValue(Creature *creature, Field *field, Population *p,
			unsigned x, unsigned y) {
		value += creature->getFenotype()->objectiveValue;
	}
};

Population::Population(Field *field) {
	this->field = field;
	productsStock.assign(World::NO_OF_PRODUCTS, 0.0);
}

Population::~Population() {
}

float Population::objectiveAvarage() {
	// TODO - nie trzeba liczyć za kazdym razem średniej.
	// Nalezy ją zapisać w cache i przy zmianie wartosci na osobniku przeliczać wartość średnią
	SumObjectiveVisitor *visitor = new SumObjectiveVisitor();
	CreaturesOfPopulationOnFieldVisitor *creatureVisitor = new CreaturesOfPopulationOnFieldVisitor(visitor);
	creatureVisitor->visit(this, NULL, -1, -1);
	float objectiveSum = visitor->getValue();
	delete creatureVisitor;
	delete visitor;
	return objectiveSum / size();

}
float Population::productNeeds(int i) {
	// Ile osobniki potrzebuja danego produktu
	NeededProductVisitor *visitor = new NeededProductVisitor(i);
	CreaturesOfPopulationOnFieldVisitor *creatureVisitor = new CreaturesOfPopulationOnFieldVisitor(visitor);
	creatureVisitor->visit(this, NULL, -1, -1);
	float needed = visitor->getValue();
	delete creatureVisitor;
	delete visitor;
	return needed;
}

float Population::resourceNeeds(int i) {
	// Ile osobniki potrzebuja danego surowca
	NeededResourceVisitor *visitor = new NeededResourceVisitor(i);
	CreaturesOfPopulationOnFieldVisitor *creatureVisitor = new CreaturesOfPopulationOnFieldVisitor(visitor);
	creatureVisitor->visit(this, NULL, -1, -1);
	float needed = visitor->getValue();
	// Jaka ilosc produktu jest dostepna w obszarze
	delete creatureVisitor;
	delete visitor;
	return needed;
}

float Population::productPrice(int i) {
	// Wartość wytworzonych produktów
	float productPrice = POSITIVE_INFINITY;
	// Jaka ilosc produktu jest dostepna w zapasach populacji
	float kept = keptProductSum(i);
	if (kept > 0)
		productPrice = this->productNeeds(i) / kept;
	return productPrice;
}

CreaturesOfPopulationOnFieldVisitor::CreaturesOfPopulationOnFieldVisitor(CreaturesOnFieldVisitor *v) :
	visitor(v) {
}

float Population::resourcePrice(int i) {
	float resourcePrice = POSITIVE_INFINITY;
	float available = field->getResourceQuantity(i);
	if (available > 0)
		resourcePrice = resourceNeeds(i) / available;
	return resourcePrice;
}

void CreaturesOfPopulationOnFieldVisitor::visit(Population *population, Field *field, unsigned x,
		unsigned y) {
	LOG4CXX_DEBUG(logger, "population:" << population << ", Population size: " << population->size());
	int popSize = population->size();
	for (int i = 0; i < popSize; i++) {
		Creature * c = (Creature *) &(population->individual(i,
				GAPopulation::RAW));
		visitor->visit(c, field, population, x, y);
	}
}

LoggerPtr CreaturesOfPopulationOnFieldVisitor::logger(Logger::getLogger("CreaturesVisitor"));

