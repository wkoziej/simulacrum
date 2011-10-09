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
#include "JSON/JSON.h"

#include "World.h"

using namespace std;

LoggerPtr World::logger(Logger::getLogger("world"));
World *World::singleton = NULL;
int World::NO_OF_RESOURCES = 3; // Liczba surowców -> 0 - natchnienie, 1 - materia
int World::NO_OF_PRODUCTS = 3; // Liczba surowców -> 0 - natchnienie, 1 - materia


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

		for (int resourceIndex = 0; resourceIndex < World::NO_OF_RESOURCES; resourceIndex++) {
			float fieldResourceQuantity = field->getResourceQuantity(
					resourceIndex);
			// TODO - nie wyliczać za każdym razem potrzebnych surowców ale brać je z cache, który będzie aktualizowany przy zmianach - podobnie z produktami/potrzebami?
			float populationResourceNeeds = population->resourceNeeds(
					resourceIndex);
			if (populationResourceNeeds > 0.0) {
				float resourceRatio = fieldResourceQuantity
						/ populationResourceNeeds;
				float availableResource = resourceRatio > 1.0 ? 1.0
						: resourceRatio;
				creature->produce(availableResource, resourceIndex);
				field->decreaseResourceQuantity(resourceIndex,
						availableResource);
			}
		}
	}
};

class SuplyVisitor: public CreaturesOnFieldVisitor {
public:
	void visit(Creature *creature, Field *field, Population *population,
			unsigned x, unsigned y) {
		creature->prepare4Meal();
		for (int productIndex = 0; productIndex < World::NO_OF_RESOURCES; productIndex++) {
			float productAmount = population->keptProductSum(productIndex);
			float productNeeds = population->productNeeds(productIndex);
			if (productNeeds > 0) {
				float productRatio = productAmount / productNeeds;
				float productAvailable = productRatio > 1.0 ? 1.0
						: productRatio;
				creature->feed(productAvailable, productIndex);
			}
		}
	}
};

class MovingVisitor: public CreaturesOnFieldVisitor {

private:
	// Logowanie
	static LoggerPtr logger;
	list<int> moves;
public:
	MovingVisitor(FieldsMatrix *fields) {
		this->fields = fields;
	}

	void visit(Creature *creature, Field *field, Population *population,
			unsigned x, unsigned y) {
		unsigned sX = x, sY = y;
		float velocity = creature->getPerformanceRatio();
		Field *destField = field;
		Population *destPopulation = population;
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
				destPopulation = World::getWorld()->findPopulationOnField(
						population->getName(), x, y);
				assert (destPopulation != NULL);
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

	void visit(Creature *creature, Field *field, Population *, unsigned x,
			unsigned y) {
		anybody = true;
	}
};

void World::createFieldsAndPopulations(unsigned X, unsigned Y) {
	World *w = World::getWorld();
	for (unsigned x = 0; x < X; x++) {
		FieldsVector fVector;
		for (unsigned y = 0; y < Y; y++) {
			Field *field = new Field();
			wstringstream name;
			name << "random" << x << "x" << y;
			NamedPopulation randomPopulationPair(name.str(), new Population());
			field->populations.insert(randomPopulationPair);
			fVector.push_back(field);
		}
		w->fields.push_back(fVector);
	}

}

World *World::createRandomWorld(unsigned X, unsigned Y) {
	if (X < 1 || Y < 1) {
		LOG4CXX_FATAL(logger, "Incorrect dimensions");
		exit(1);
	}
	World *world = World::getWorld();
	world->createFieldsAndPopulations(X, Y);
	world->initializeRandomly();
	world->createRandomCreatures();
	return world;
}

World *World::readWorldFromFile(const char *fileName) {

	std::string fileContent;
	World *world = NULL;
	ifstream file;
	file.open(fileName);
	char output[100];
	LOG4CXX_DEBUG(logger, "fileName: " << fileName);

	if (file.is_open()) {

		while (!file.eof()) {
			file >> output;
			fileContent.append(output);
		}

		LOG4CXX_DEBUG(logger, "fileContent:" << fileContent);
		JSONValue *value = JSON::Parse(fileContent.c_str());
		assert (value != NULL);
		if (value != NULL) {
			JSONObject root = value->AsObject();
			root = root[L"world"]->AsObject();
			int X = root.at(L"sizeX")->AsNumber();
			int Y = root.at(L"sizeY")->AsNumber();
			LOG4CXX_DEBUG(logger, "wordlSize (" << X << "," << Y << ")");
			World::NO_OF_PRODUCTS = root.at(L"productsNo")->AsNumber();
			LOG4CXX_DEBUG(logger, "NO_OF_PRODUCTS :" << World::NO_OF_PRODUCTS);
			World::NO_OF_RESOURCES = root.at(L"resourcesNo")->AsNumber();
			LOG4CXX_DEBUG(logger, "NO_OF_RESOURCES :" << World::NO_OF_RESOURCES);
			world = World::getWorld();
			for (int i = 0; i < X; i++) {
				FieldsVector fVector;
				for (int j = 0; j < Y; j++) {
					std::wstringstream fieldName;
					fieldName << "field" << i << "x" << j;
					LOG4CXX_DEBUG(logger, L" field : " << fieldName);
					Field *field = NULL;
					if (root.count(fieldName.str()) > 0) {
						JSONObject JSONfield =
								root.at(fieldName.str())->AsObject();
						field = new Field(JSONfield);
						if (JSONfield.count(L"populations") > 0) {
							JSONArray populations =
									JSONfield.at(L"populations")->AsArray();
							JSONArray::iterator population =
									populations.begin();
							LOG4CXX_DEBUG(logger, "loading populations : " << populations.size());
							for (; population != populations.end(); population++) {
								Population *populationObject = new Population(
										(*population)->AsObject());
								NamedPopulation namedPopulation(
										populationObject->getName(),
										populationObject);
								field->populations.insert(namedPopulation);
							}
						}
					}
					assert(field != NULL);
					fVector.push_back(field);

					for (int r = 0; r < World::NO_OF_RESOURCES; r++) {
						//float productPrice(int i);
						float price = field->resourcePrice(r);
						LOG4CXX_DEBUG(logger, "resource [" << r << "]: price = " << price);
					}
					for (int p = 0; p < World::NO_OF_PRODUCTS; p++) {
						//float productPrice(int i);
						float price = field->productPrice(p);
						LOG4CXX_DEBUG(logger, "product [" << p << "]: price = " << price);
					}
				}
				world->fields.push_back(fVector);
			}

			LOG4CXX_DEBUG(logger, "");
		}

	}
	return world;
}

void World::initializeRandomly() {
	LOG4CXX_TRACE(logger, "initializeRandomly");
	FieldsMatrix::iterator i = getWorld()->fields.begin();
	for (; i != getWorld()->fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			(*j)->initializeRandomly();
		}
	}
}

void World::createRandomCreatures() {
	LOG4CXX_TRACE(logger, "createCreatures");
	// Wybierz losowe pole
	for (int z = 0; z < NO_OF_POPULATIONS; z++) {
		unsigned x = (random() / (float) RAND_MAX) * getWorld()->fields.size();
		unsigned y = (random() / (float) RAND_MAX)
				* getWorld()->fields.begin()->size();
		LOG4CXX_DEBUG(logger, "Initial Field: x  " << x << ", y " << y);
		Population *population =
				getWorld()->fields.at(x).at(y)->populations.begin()->second;
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
	CreaturesOnFieldVisitor * visitor = new MovingVisitor(&fields);
	iterateCreaturesOnFields(visitor);
	delete visitor;
}

void World::resourcesRenovation() {
	LOG4CXX_TRACE(logger, "resourcesRenovation");
	FieldsMatrix::iterator i = getWorld()->fields.begin();
	for (; i != getWorld()->fields.end(); i++) {
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
	creaturesSupplying();
	creaturesWorking();
	creaturesMoving();
	creaturesReproducting();
	creaturesDying();

}

World::~World() {
	LOG4CXX_TRACE(logger, "~World");
}

void World::iterateCreaturesOnFields(CreaturesOnFieldVisitor *visitor) {
	LOG4CXX_TRACE(logger, "iterateCreaturesOnFields");
	PopulationOnFieldVisitor *creaturesVisitor =
			new CreaturesOfPopulationOnFieldVisitor(visitor);
	iteratePopulationOnFields(creaturesVisitor);
	delete creaturesVisitor;
}

void World::iteratePopulationOnFields(PopulationOnFieldVisitor *visitor) {
	LOG4CXX_TRACE(logger, "iteratePopulationOnFields");
	FieldsMatrix::iterator i = getWorld()->fields.begin();
	for (; i != getWorld()->fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			unsigned x = i - getWorld()->fields.begin();
			unsigned y = j - i->begin();
			PopulationsMap::iterator p = (*j)->populations.begin();
			for (; p != (*j)->populations.end(); p++) {
				Population *population = p->second;
				visitor->visit(population, *j, x, y);
			}
		}
	}
}

Population *World::findPopulationOnField(std::wstring name, int x, int y) {
	Population *population = NULL;
	Field *field = fields.at(x).at(y);
	PopulationsMap::iterator i = field->populations.find(name);
	if (i != field->populations.end()) {
		population = i->second;
	}
	return population;
}
