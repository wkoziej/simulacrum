/*
 * World.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#include <numeric>
#include <utility>
#include <stdlib.h>
#include <ga/GASimpleGA.h>

#include "World.h"

using namespace std;

LoggerPtr World::logger(Logger::getLogger("world"));
World *World::singleton = NULL;

class ReproductionVisitor: public PopulationOnFieldVisitor {
private:
	// Logowanie
	static LoggerPtr logger;
public:
	virtual void visit(Population *population, Field *field, unsigned x,
			unsigned y) {
		LOG4CXX_DEBUG(logger, "Population size before reproduction: " << population->size());
		int c;
		for (c = 0; c < population->size(); c++) {
			LOG4CXX_DEBUG(logger, " I [" << c << "]: " << population->individual(c) << " UD: " << population->individual(c).evalData());
		}
		if (population->size()) {
			GASimpleGA simpleGA(*population);
			//simpleGA.objectiveFunction();
			simpleGA.pMutation(MUTATION);
			simpleGA.pCrossover(CROSSOVER);
			simpleGA.step();
			GAPopulation p = simpleGA.population();

			int maxInd = p.size() > 2 ? 2 : p.size();
			for (c = 0; c < maxInd; c++) {
				LOG4CXX_DEBUG(logger, " J [" << c << "]: " << p.individual(c) << " UD: " << p.individual(c).evalData());
				population->add(p.individual(c));
			}
		}
		LOG4CXX_DEBUG(logger, "Population size after reproduction: " << population->size());

	}
};

LoggerPtr ReproductionVisitor::logger(Logger::getLogger("ReproductionVisitor"));

class DyingVisitor: public PopulationOnFieldVisitor {

private:
	// Logowanie
	static LoggerPtr logger;
public:
	void visit(Population *population, Field *field, unsigned x, unsigned y) {
		int psize = population->size();
		for (int c = 0; c < population->size();) {
			CreatureFenotype *f =
					(CreatureFenotype *) population->individual(c).evalData();
			// Jezeli wartosc funkcji celu jest mniejsza od sredniej populacji, to wyrzuc osobnika z populacji
			// Pomijaj dzieci
			float objAvg = population->objectiveAvarage();
			float creatureObjVal = f->objectiveValue;
			if (objAvg > creatureObjVal && f->yearsOld > 0) {
				population->remove(&(population->individual(c)));
			} else {
				c++;
			}
		}
		LOG4CXX_DEBUG(logger, "population size get smaller from " << psize << " to " << population->size ());
	}
};

LoggerPtr DyingVisitor::logger(Logger::getLogger("DyingVisitor"));

class WorkerVisitor: public CreaturesOnFieldVisitor {
public:
	void visit(Creature *creature, Field *field, Population *population,
			unsigned x, unsigned y) {
		for (int resourceIndex = 0; resourceIndex < NO_OF_RESOURCES; resourceIndex++) {
			float fieldResourceQuantity = field->getResourceQuantity(
					resourceIndex);
			// TODO - nie wyliczać za każdym razem potrzebnych surowców ale brać je z cache, który będzie aktualizowany przy zmianach - podobnie z produktami/potrzebami?
			float populationResourceNeeds = population->resourceNeeds(
					resourceIndex);
			float resourceRatio = fieldResourceQuantity
					/ populationResourceNeeds;
			float availableResource = resourceRatio > 1.0 ? 1 : resourceRatio;
			float creatureResourceNeeds = creature->getNeedOfResource(
					resourceIndex);
			float resourceToUse = availableResource * creatureResourceNeeds;
			//float productQuantity =
			creature->produce(resourceToUse, resourceIndex);
			//field->increaseProductQuantity(resourceIndex, productQuantity);
			field->decreaseResourceQuantity(resourceIndex, resourceToUse);
		}
	}
};

class SuplyVisitor: public CreaturesOnFieldVisitor {
public:
	void visit(Creature *creature, Field *field, GAPopulation *population,
			unsigned x, unsigned y) {
		for (unsigned productIndex = 0; productIndex < NO_OF_RESOURCES; productIndex++) {
			float productAmount = field->getProductQuantity(productIndex);
			float productEaten;
			bool amountSuffice;
			creature->feed(productAmount, productEaten, amountSuffice,
					productIndex);
			field->decreaseProductQuantity(productEaten, productIndex);
		}
	}
};

class MovingVisitor: public CreaturesOnFieldVisitor {

private:
	// Logowanie
	static LoggerPtr logger;
	list<int> moves;
public:
	MovingVisitor(FieldsMatrix *fields, PopulationsMatrix *populations) {
		this->fields = fields;
		this->populations = populations;
	}

	void visit(Creature *creature, Field *field, GAPopulation *population,
			unsigned x, unsigned y) {
		unsigned sX = x, sY = y;
		float velocity = creature->getVelocity();
		Field *destField = field;
		GAPopulation *destPopulation = population;
		int step = 0;
		Creature::Directions nextDirection = Creature::NoDirection;
		CreatureFenotype *f = (CreatureFenotype *) creature->evalData();
		LOG4CXX_DEBUG(logger, " start coord  (" << x << "," << y <<") of field " << field << " - YEARS :" << f->yearsOld);
		if (f->yearsOld > 0) {
			bool getOut, hasNextDirection;
			do {
				getOut = destField->getOut(velocity);
				if (getOut) {
					nextDirection = creature->nextDirection(step);
					hasNextDirection = nextDirection != Creature::NoDirection;
					if (hasNextDirection) {
						calculateNextCoordinates(x, y, nextDirection);
						destField = fields->at(x).at(y);
						LOG4CXX_DEBUG(logger, "next coord  (" << x << "," << y <<") and field = " << destField);
					}
				}
			} while (getOut && hasNextDirection);

			if (destField != field) {
				// Ruszyliśmy się
				destPopulation = populations->at(x).at(y);
				population->remove(creature);
				destPopulation->add(creature);
				f->yearsOld = 0;
				f->previousFieldIdexes = pair<unsigned, unsigned> (sX, sY);
				LOG4CXX_DEBUG(logger, "move from (" << sX << "," << sY << ") to (" << x << "," << y <<")");

				LOG4CXX_DEBUG(logger, "destPopulation->size " << destPopulation->size() << " population->size " << population->size());
			} else {
				f->yearsOld++;
			}
		} else {
			f->yearsOld++;
		}
	}
private:
	FieldsMatrix *fields;
	PopulationsMatrix *populations;

	void calculateNextCoordinates(unsigned &x, unsigned &y,
			Creature::Directions direction) {
		switch (direction) {
		case Creature::DirLeft: {
			if (x == 0) {
				x = fields->size() - 1;
			} else {
				x--;
			}
			break;
		}
		case Creature::DirRight: {
			if (x == fields->size() - 1) {
				x = 0;
			} else {
				x++;
			}
			break;
		}
		case Creature::DirUp: {
			if (y == 0) {
				y = fields->at(0).size() - 1;
			} else {
				y--;
			}
			break;
		}
		case Creature::DirDown: {
			if (y == fields->at(0).size() - 1) {
				y = 0;
			} else {
				y++;
			}
			break;
		}
		default:
			break;
		}
	}
};

LoggerPtr MovingVisitor::logger(Logger::getLogger("MovingVisitor"));

class AnybodyOutThereVisitor: public CreaturesOnFieldVisitor {
public:
	bool anybody;
	AnybodyOutThereVisitor() {
		anybody = false;
	}

	void visit(Creature *creature, Field *field, GAPopulation *, unsigned x,
			unsigned y) {
		anybody = true;
	}
};

World *World::createWorld(unsigned X, unsigned Y) {
	if (X < 1 || Y < 1) {
		LOG4CXX_FATAL(logger, "Incorrect dimensions");
		exit(1);
	}
	World *w = World::singleton;
	for (unsigned x = 0; x < X; x++) {
		FieldsVector fVector;
		//CreaturesListsVector cVector;
		PopulationsVector pVector;
		for (unsigned y = 0; y < Y; y++) {
			fVector.push_back(new Field());
			//CreaturesList cList;
			pVector.push_back(new Population());
		}
		fields.push_back(fVector);
		populations.push_back(pVector);
	}

}

void World::initializeRandomly() {
	LOG4CXX_TRACE(logger, "initializeRandomly");
	FieldsMatrix::iterator i = fields.begin();
	for (; i != fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			(*j)->initializeRandomly();
		}
	}
}
void World::createCreatures() {
	LOG4CXX_TRACE(logger, "createCreatures");
	// Wybierz losowe pole
	for (int z = 0; z < NO_OF_POPULATIONS; z++) {
		unsigned x = (random() / (float) RAND_MAX) * fields.size();
		unsigned y = (random() / (float) RAND_MAX) * fields.begin()->size();
		LOG4CXX_DEBUG(logger, "Initial Field: x  " << x << ", y " << y);
		GAPopulation *population = populations.at(x).at(y);
		for (int i = 0; i < INITIAL_NO_OF_CREATURES_IN_FIELD; i++) {
			Creature * creature = new Creature();
			population->add(creature);
		};
		LOG4CXX_DEBUG(logger, "population: " << population << ", creaturesCreated: " << population->size());

		for (int c = 0; c < population->size(); c++) {
			LOG4CXX_DEBUG(logger, " creature [" << c << "]: " << population->individual(c) << " UD: " << population->individual(c).evalData());
		}
	}
}

void World::creaturesWorking() {
	LOG4CXX_TRACE(logger, "creaturesWorking");
	CreaturesOnFieldVisitor * visitor = new WorkerVisitor();
	iterateCreaturesOnFields(visitor);
	delete visitor;
}

void World::creaturesSupplying() {
	LOG4CXX_TRACE(logger, "creaturesSupplying");
	CreaturesOnFieldVisitor * visitor = new SuplyVisitor();
	iterateCreaturesOnFields(visitor);
	delete visitor;
}
void World::creaturesDying() {
	LOG4CXX_TRACE(logger, "creaturesDying");
	PopulationOnFieldVisitor * visitor = new DyingVisitor();
	iteratePopulationOnFields(visitor);
	delete visitor;
}

void World::creaturesReproducting() {
	LOG4CXX_TRACE(logger, "creaturesReproducting");
	PopulationOnFieldVisitor *visitor = new ReproductionVisitor();
	iteratePopulationOnFields(visitor);
	delete visitor;
}

void World::creaturesMoving() {
	LOG4CXX_TRACE(logger, "creaturesMoving");
	CreaturesOnFieldVisitor * visitor =
			new MovingVisitor(&fields, &populations);

	iterateCreaturesOnFields(visitor);

	delete visitor;
}

void World::resourcesRenovation() {
	LOG4CXX_TRACE(logger, "resourcesRenovation");
	FieldsMatrix::iterator i = fields.begin();
	for (; i != fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			(*j)->renovateResources();
		}
	}
}

bool World::creaturesExists() {
	AnybodyOutThereVisitor * visitor = new AnybodyOutThereVisitor();
	iterateCreaturesOnFields(visitor);
	bool anybody = visitor->anybody;
	delete visitor;
	return anybody;
}

void World::step() {
	resourcesRenovation();
	creaturesWorking();
	creaturesSupplying();
	creaturesDying();
	creaturesReproducting();
	creaturesMoving();
}

World::~World() {
	LOG4CXX_TRACE(logger, "~World");
}

void World::iterateCreaturesOnFields(CreaturesOnFieldVisitor *visitor) {
	LOG4CXX_TRACE(logger, "iterateCreaturesOnFields");
	PopulationOnFieldVisitor *creaturesVisitor = new CreaturesVisitor(visitor);
	iteratePopulationOnFields(creaturesVisitor);
	delete creaturesVisitor;
}

void World::iteratePopulationOnFields(PopulationOnFieldVisitor *visitor) {
	LOG4CXX_TRACE(logger, "iteratePopulationOnFields");
	FieldsMatrix::iterator i = fields.begin();
	for (; i != fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			unsigned x = i - fields.begin();
			unsigned y = j - i->begin();
			GAPopulation *population = populations.at(x).at(y);
			visitor->visit(population, *j, x, y);
		}
	}
}

