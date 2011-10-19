/*
 * World.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#include <numeric>
#include <utility>
#include <list>
#include <stdlib.h>
#include <algorithm>

#include <ga/GASimpleGA.h>
#include "JSON/JSON.h"
#include "../StateSaver.h"
#include <QtCore/QThread>
#include "World.h"
#include "Article.h"
#include "Recipe.h"

using namespace std;

LoggerPtr World::logger(Logger::getLogger("world"));
World *World::singleton = NULL;
unsigned World::NO_OF_ARTICLES = 5;
unsigned World::HEIGHT = 1;
unsigned World::WIDTH = 1;
std::vector<std::string> World::ARTICLES;
std::vector<Article *> World::articles;
std::vector<Recipe *> World::recipes;

int World::getArticleIndex(std::wstring articleName) {
	int index = -1;
	std::string name = wstring2string(articleName);
	std::vector<std::string>::iterator i = find(World::ARTICLES.begin(),
			World::ARTICLES.end(), name);
	if (i != World::ARTICLES.end()) {
		index = i - World::ARTICLES.begin();
	}
	return index;
}

class CreatureActivityThread: public QThread {
public:
	CreatureActivityThread(Creature *creature);
	virtual ~CreatureActivityThread();
	void run();
private:
	Creature *creature;
};

CreatureActivityThread::CreatureActivityThread(Creature *creature) :
	QThread() {
	this->creature = creature;
}

CreatureActivityThread::~CreatureActivityThread() {
	// TODO Auto-generated destructor stub
}

void CreatureActivityThread::run() {
	// Wyznacz czynność do wykonania i
	//this->creature->doAllActivities();
	CreatureActivityList activities = creature->getCreatureActivities();
	CreatureActivityList::iterator activity = activities.begin();
	for (; activity != activities.end(); activity++) {
		(*activity)->make();
		//this->msleep(100);
		delete (*activity);
	}
}

class ReproductionVisitor: public PopulationOnFieldVisitor {
private:
	// Logowanie
	static LoggerPtr logger;
public:
	virtual void visit(CreaturesPopulation *population, Field *field,
			unsigned x, unsigned y) {
		LOG4CXX_DEBUG(logger, "Population size before reproduction: " << population->size());
		int c;
		int psize = population->size();
		for (c = 0; c < psize; c++) {
			GAGenome g = population->individual(c);
			LOG4CXX_DEBUG(logger, " I [" << c << "]: " << population->individual(c) << " UD: " << population->individual(c).evalData());
		}

		if (psize > 7) {
			population->remove(&population->worst());
		}

		if (population->size()) {
			GASimpleGA simpleGA(*population);
			simpleGA.initialize(time(0));
			simpleGA.pMutation(MUTATION);
			simpleGA.pCrossover(CROSSOVER);
			simpleGA.step();
			GAPopulation p = simpleGA.population();
			for (c = 0; c < p.size(); c++) {
				GAGenome g = p.individual(c);
				LOG4CXX_DEBUG(logger, " N [" << c << "]: " << p.individual(c) << " UD: " << p.individual(c).evalData());
			}

			//int maxInd = p.size();
			//for (c = 0; c < maxInd; c++) {
			//LOG4CXX_DEBUG(logger, " J [" << c << "]: " << p.individual(c) << " UD: " << p.individual(c).evalData());

			population->add(new Creature((Creature&) p.best()));
			//}
		}
		LOG4CXX_DEBUG(logger, "Population size after reproduction: " << population->size());
	}
};

LoggerPtr ReproductionVisitor::logger(Logger::getLogger("ReproductionVisitor"));
/*

 class DyingVisitor: public PopulationOnFieldVisitor {

 private:
 // Logowanie
 static LoggerPtr logger;
 public:
 void visit(CreaturesPopulation *population, Field *field, unsigned x,
 unsigned y) {
 int psize = population->size();

 for (int c = 0; c < population->size() && population->size() > 2;) {
 //Creature *creature = (Creature *) &population->individual(c);
 // Jezeli wartosc funkcji celu jest mniejsza od sredniej populacji, to wyrzuc osobnika z populacji
 // Pomijaj dzieci
 /*float objAvg = population->objectiveAvarage();
 float creatureObjVal = population->individual(c).score();
 LOG4CXX_DEBUG(logger, "population->individual("<<c<<") " << population->individual(c) << ", objective: " << creatureObjVal);
 if (objAvg > creatureObjVal && creature->getAge() > 0) {
 population->remove(&(population->individual(c)));
 } else {
 c++;
 }

 }
 if (psize > 7) {
 population->remove(&population->worst());
 }
 LOG4CXX_DEBUG(logger, "population size get smaller from " << psize << " to " << population->size ());
 }
 };

 LoggerPtr DyingVisitor::logger(Logger::getLogger("DyingVisitor"));
 */

class ObjectiveEvaluatorVisitor: public PopulationOnFieldVisitor {

private:
	// Logowanie
	static LoggerPtr logger;
public:
	void visit(CreaturesPopulation *population, Field *field, unsigned x,
			unsigned y) {
		population->evaluate(gaTrue);
		LOG4CXX_DEBUG(logger, L"population " << population->getName() << L" evaluated");
	}
};

LoggerPtr ObjectiveEvaluatorVisitor::logger(Logger::getLogger(
		"ObjectiveEvaluatorVisitor"));

class CreatureActivityVisitor: public PopulationOnFieldVisitor {
private:
	// Logowanie
	static LoggerPtr logger;
public:
	void visit(CreaturesPopulation *population, Field *field, unsigned x,
			unsigned y) {
		std::list<CreatureActivityThread *> threadsToWait;
		unsigned popSize = population->size();
		for (unsigned i = 0; i < popSize; i++) {
			CreatureActivityThread *activityThread =
					new CreatureActivityThread(
							(Creature*) &population->individual(i));
			activityThread->start(QThread::NormalPriority);
			threadsToWait.push_back(activityThread);
		}
		while (threadsToWait.size()) {
			threadsToWait.front()->wait();
			threadsToWait.pop_front();
		}
	}
};
LoggerPtr CreatureActivityVisitor::logger(Logger::getLogger(
		"CreatureActivityVisitor"));

class AnybodyOutThereVisitor: public CreaturesOnFieldVisitor {
public:
	bool anybody;
	AnybodyOutThereVisitor() {
		anybody = false;
	}

	void visit(Creature *creature, Field *field, CreaturesPopulation *,
			unsigned x, unsigned y) {
		anybody = true;
	}
};

/*void World::createFieldsAndPopulations(unsigned X, unsigned Y) {
 World *w = World::getWorld();
 for (unsigned x = 0; x < X; x++) {
 FieldsVector fVector;
 for (unsigned y = 0; y < Y; y++) {
 Field *field = new Field();
 wstringstream name;
 name << "random" << x << "x" << y;
 NamedPopulation randomPopulationPair(name.str(),
 new CreaturesPopulation());
 field->populations.insert(randomPopulationPair);
 fVector.push_back(field);
 }
 w->fields.push_back(fVector);
 }

 }*/

/*World *World::createRandomWorld(unsigned X, unsigned Y) {
 if (X < 1 || Y < 1) {
 LOG4CXX_FATAL(logger, "Incorrect dimensions");
 exit(1);
 }
 World *world = World::getWorld();
 world->createFieldsAndPopulations(X, Y);
 world->initializeRandomly();
 world->createRandomCreatures();
 return world;
 }*/

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
			World::HEIGHT = Y;
			World::WIDTH = X;
			LOG4CXX_DEBUG(logger, "wordlSize (" << X << "," << Y << ")");
			JSONArray articles = root.at(L"articles")->AsArray();
			World::NO_OF_ARTICLES = articles.size();

			LOG4CXX_DEBUG(logger, "NO_OF_RESOURCES :" << World::NO_OF_ARTICLES);
			JSONArray::iterator a = articles.begin();
			for (; a != articles.end(); a++) {
				JSONObject article = (*a)->AsObject();

				/*std::wstring articleName = article.begin()->first;
				 std::string name(articleName.length(), ' ');
				 std::copy(articleName.begin(), articleName.end(), name.begin());*/
				std::string name = wstring2string(article.begin()->first);
				World::ARTICLES.push_back(name);
				JSONObject articleProps = article.begin()->second->AsObject();
				Article *articleObject = new Article();
				articleObject->setFood(articleProps.count(L"isFood?") > 0
						&& articleProps.at(L"isFood?")->AsBool());
				World::articles.push_back(articleObject);
			}

			JSONArray recipes = root.at(L"recipes")->AsArray();
			World::recipes.assign(World::NO_OF_ARTICLES, NULL);
			JSONArray::iterator r = recipes.begin();
			for (; r != recipes.end(); r++) {
				JSONObject recipe = (*r)->AsObject();
				std::wstring articleName = recipe.begin()->first;
				int articleIndex = World::getArticleIndex(articleName);
				assert (articleIndex != -1);
				Recipe *recipeObject = new Recipe();
				UnsignedVector ingredients;
				ingredients.assign(World::NO_OF_ARTICLES, 0);
				JSONArray recipeIngredients = recipe.begin()->second->AsArray();
				JSONArray::iterator g = recipeIngredients.begin();
				for (; g != recipeIngredients.end(); g++) {
					JSONObject ingredient = (*g)->AsObject();
					std::wstring ingredientName = ingredient.begin()->first;
					int ingredientIndex =
							World::getArticleIndex(ingredientName);
					assert (ingredientIndex != -1);
					JSONValue value = ingredient.begin()->second->AsNumber();
					ingredients.at(ingredientIndex) = value.AsNumber();
				}
				World::recipes.push_back(recipeObject);
			}

			world = World::getWorld();
			for (int i = 0; i < X; i++) {
				FieldsVector fVector;
				for (int j = 0; j < Y; j++) {
					std::wstringstream fieldName;
					fieldName << "field" << i << "x" << j;
					LOG4CXX_DEBUG(logger, L" field : " << fieldName.str());
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
								CreaturesPopulation
										*populationObject =
												new CreaturesPopulation(
														field,
														(*population)->AsObject(),
														i, j);
								// populations.insert(namedPopulation)
								field->addPopulation(populationObject);
							}
						}
					}
					assert(field != NULL);
					fVector.push_back(field);
				}
				world->fields.push_back(fVector);
			}
			LOG4CXX_DEBUG(logger, "");
		}
		file.close();
	}
	return world;
}
/*
 void World::initializeRandomly() {
 LOG4CXX_TRACE(logger, "initializeRandomly");
 FieldsMatrix::iterator i = getWorld()->fields.begin();
 for (; i != getWorld()->fields.end(); i++) {
 FieldsVector::iterator j = i->begin();
 for (; j != i->end(); j++) {
 (*j)->initializeRandomly();
 }
 }
 }*/

/*void World::createRandomCreatures() {
 LOG4CXX_TRACE(logger, "createCreatures");
 // Wybierz losowe pole
 for (int z = 0; z < NO_OF_POPULATIONS; z++) {
 unsigned x = (random() / (float) RAND_MAX) * getWorld()->fields.size();
 unsigned y = (random() / (float) RAND_MAX)
 * getWorld()->fields.begin()->size();
 LOG4CXX_DEBUG(logger, "Initial Field: x  " << x << ", y " << y);
 Field *field = getWorld()->fields.at(x).at(y);
 CreaturesPopulation *population = field->populations.begin()->second;
 for (int i = 0; i < INITIAL_NO_OF_CREATURES_IN_FIELD; i++) {
 Creature * creature = new Creature(population, field);
 population->add(creature);
 };
 LOG4CXX_DEBUG(logger, "population: " << population << ", creaturesCreated: " << population->size());
 for (int c = 0; c < population->size(); c++) {
 LOG4CXX_DEBUG(logger, " creature [" << c << "]: " << population->individual(c) << " UD: " << population->individual(c).evalData());
 }
 }
 }*/

/*void World::creaturesDying() {
 LOG4CXX_TRACE(logger, "creaturesDying");
 PopulationOnFieldVisitor * visitor = new DyingVisitor();
 iteratePopulationOnFields(visitor);
 delete visitor;
 }*/

void World::creaturesReproducting() {
	LOG4CXX_TRACE(logger, "creaturesReproducting");
	PopulationOnFieldVisitor *visitor = new ReproductionVisitor();
	iteratePopulationOnFields(visitor);
	delete visitor;
}

void World::nextYear() {
	LOG4CXX_TRACE(logger, "nextYear");
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

void World::step(StateSaver *stateSaver) {
	PopulationOnFieldVisitor * visitor = new CreatureActivityVisitor();
	iteratePopulationOnFields(visitor);
	delete visitor;
	stateSaver->save("activitiesMade");

	// Na podstawie funkcji celu dokonaj roznmożenia
	creaturesReproducting();
	stateSaver->save("reproductionDone");
	// Usuń z populacji najmniej przystosowane
	/*	creaturesDying();
	 stateSaver->save("dieAfterDie");*/
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
			std::list<CreaturesPopulation *> populations =
					(*j)->getPopulations();
			std::list<CreaturesPopulation *>::iterator p = populations.begin();
			for (; p != populations.end(); p++) {
				visitor->visit(*p, *j, x, y);
			}
		}
	}
}

void World::visitFields(FieldsVisitor *fieldVisitor) {
	FieldsMatrix::iterator i = getWorld()->fields.begin();
	for (; i != getWorld()->fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			fieldVisitor->visit(*j);
		}
	}

}

CreaturesPopulation *World::findOrCreatePopulation(
		const CreaturesPopulation *species, int x, int y) {
	Field *field = fields.at(x).at(y);
	CreaturesPopulation *population = field->getPopulation(species->getName());
	if (population == NULL) {
		population = new CreaturesPopulation(species);
		field->addPopulation(population);
	}
	return population;
}
