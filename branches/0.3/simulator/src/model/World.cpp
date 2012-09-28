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

#include "JSON/JSON.h"
#include "../StateSaver.h"
#include <QtCore/QThread>
#include "World.h"
#include "Article.h"
#include "Recipe.h"
#include "Activity.h"
#include "Creature.h"
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

void World::connectCreatureSignals(Creature *creature) {
	/*
	 creature ->connect(
	 creature,
	 SIGNAL(creatureBorn (QString, unsigned, unsigned)),
	 VisualizationController::getPtr(),
	 SLOT(assignAvatarToNewCreature (QString, unsigned, unsigned ))
	 );
	 creature ->connect(
	 creature,
	 SIGNAL(creatureDie (QString, unsigned, unsigned)),
	 VisualizationController::getPtr(),
	 SLOT(releaseAvatar (QString, unsigned, unsigned ))
	 );
	 creature ->connect(
	 creature,
	 SIGNAL(creatureMoved (QString , unsigned , unsigned , unsigned , unsigned )),
	 VisualizationController::getPtr(),
	 SLOT(changeCreaturePosition (QString , unsigned , unsigned , unsigned , unsigned ))
	 );
	 creature ->connect(
	 creature, SIGNAL(creatureAte (QString, unsigned)),
	 VisualizationController::getPtr(),
	 SLOT(feedCreature (QString, unsigned ))
	 );
	 */
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
}

void CreatureActivityThread::run() {
	// Wyznacz czynność do wykonania i
	//this->creature->doAllActivities();
	ActivityList activities = CreatureActivity::getCreatureActivities(creature);
	ActivityList::iterator activity = activities.begin();
	for (; activity != activities.end(); activity++) {
		(*activity)->make();
		this->msleep(100);
		delete (*activity);
	}
}

class CreatureGetOldFieldVisitor : public CreaturesOnFieldVisitor {
public:

	virtual void visit(Creature *creature, Field *field, CreaturesPopulation *,
			unsigned x, unsigned y)  {
		creature->increaseAge();
	}

};

class ReproductionVisitor: public PopulationOnFieldVisitor {
private:
	// Logowanie
	static LoggerPtr logger;
public:
	virtual void visit(CreaturesPopulation *population, Field *field,
			unsigned x, unsigned y) {

		LOG4CXX_DEBUG(logger,
				"Population size before reproduction: " << population->size());

		population->reproduce();
		/*

		 int c;
		 int psize = population->size();
		 for (c = 0; c < psize; c++) {
		 Creature * creature = (Creature *) &population->individual(c);
		 creature->increaseAge();
		 LOG4CXX_DEBUG(logger, " I [" << c << "]: " << population->individual(c));
		 }

		 if (population->size()) {
		 GASimpleGA simpleGA(*population);
		 simpleGA.initialize(time(0));
		 simpleGA.crossover(GARealTwoPointCrossover);
		 simpleGA.pMutation(MUTATION);
		 simpleGA.pCrossover(CROSSOVER);
		 simpleGA.step();
		 GAPopulation p = simpleGA.population();
		 psize = p.size() / 2;
		 for (c = 0; c < psize; c++) {
		 Creature *theBest = new Creature((Creature&) p.best(c));
		 population->add(theBest);
		 LOG4CXX_DEBUG(logger, " N [" << c << "]: " << p.individual(c));
		 }
		 }

		 psize = population->size();
		 for (c = psize - 30; c > 0; c--) {
		 Creature * creature = (Creature *) &population->worst();
		 if (creature->getAge() > 0) {
		 creature->prepareToDie();
		 population->remove(creature);
		 // disconnect all signals
		 // delete creature

		 }
		 }
		 */

		LOG4CXX_DEBUG(logger,
				"Population size after reproduction: " << population->size());
	}
};

LoggerPtr ReproductionVisitor::logger(Logger::getLogger("ReproductionVisitor"));

// TODO: Wyliczanie wartości powinno zadziać sie autoamtycznie... dlatego komentuję
/*
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
 */

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
			CreatureActivityThread *activityThread = new CreatureActivityThread(
					(Creature*) &population->individual(i));
			activityThread->start(QThread::NormalPriority);
			threadsToWait.push_back(activityThread);
		}

		while (threadsToWait.size()) {
			LOG4CXX_DEBUG(logger,
					"waiting for all threads, lost = " << threadsToWait.size());
			threadsToWait.front()->wait();
			threadsToWait.pop_front();
		}

		LOG4CXX_TRACE(logger, "all threads completed...");
	}
};
LoggerPtr CreatureActivityVisitor::logger(
		Logger::getLogger("CreatureActivityVisitor"));

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

World *World::readWorldFromFile(const char *fileName) {
	Activity::init();
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
		assert(value != NULL);
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
				articleObject->setFood(
						articleProps.count(L"isFood?") > 0
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
				assert(articleIndex != -1);
				Recipe *recipeObject = new Recipe();
				UnsignedVector ingredients;
				ingredients.assign(World::NO_OF_ARTICLES, 0);
				JSONArray recipeIngredients = recipe.begin()->second->AsArray();
				JSONArray::iterator g = recipeIngredients.begin();
				for (; g != recipeIngredients.end(); g++) {
					JSONObject ingredient = (*g)->AsObject();
					std::wstring ingredientName = ingredient.begin()->first;
					int ingredientIndex = World::getArticleIndex(
							ingredientName);
					assert(ingredientIndex != -1);
					JSONValue value = ingredient.begin()->second->AsNumber();
					ingredients.at(ingredientIndex) = value.AsNumber();
					recipeObject->setIngredientsVector(ingredients);
				}
				World::recipes.at(articleIndex) = recipeObject;
			}

			world = World::getPtr();
			int i;
			for (i = 0; i < X; i++) {
				FieldsVector fVector;
				fVector.assign(Y, NULL);
				world->fields.push_back(fVector);
			}

			JSONArray fields = root.at(L"fields")->AsArray();
			JSONArray::iterator f = fields.begin();
			for (; f != fields.end(); f++) {
				Field *field = NULL;
				JSONObject JSONfield = (*f)->AsObject();
				unsigned fieldX = JSONfield.at(L"coordX")->AsNumber();
				unsigned fieldY = JSONfield.at(L"coordY")->AsNumber();
				// Czy gdzieś się nie powtórzyliśmy?
				assert(world->fields.at(fieldX).at(fieldY)== NULL);
				field = new Field(JSONfield);
				if (JSONfield.count(L"populations") > 0) {
					JSONArray populations =
							JSONfield.at(L"populations")->AsArray();
					JSONArray::iterator population = populations.begin();
					LOG4CXX_DEBUG(logger,
							"loading populations : " << populations.size());
					for (; population != populations.end(); population++) {
						CreaturesPopulation *populationObject =
								new CreaturesPopulation(field,
										(*population)->AsObject(), fieldX,
										fieldY);
						// populations.insert(namedPopulation)
						field->addPopulation(populationObject);
						// Dodajemy do naszego algorytmu genetycznego
					}
				}
				assert(field != NULL);
				world->fields.at(fieldX).at(fieldY) = field;
			}

			// Reszta pol pustych
			for (i = 0; i < X; i++) {
				for (int j = 0; j < Y; j++) {
					if (world->fields[i][j] == NULL) {
						world->fields[i][j] = new Field();
					}
				}
			}
		}
		file.close();
	}
	return world;
}

void World::creaturesReproducting() {
	LOG4CXX_TRACE(logger, "creaturesReproducting");
	// TODO: Tutaj wszystkie populacje powinny wykonać jeden krok wprzód
	// - narodziny nowych osobników
	// - migracja
	// - umieranie -> wypieranie przez nowych

	// GAIncrementalGA
	PopulationOnFieldVisitor *visitor = new ReproductionVisitor();
	iteratePopulationOnFields(visitor);
	delete visitor;
}

void World::nextYear() {
	LOG4CXX_TRACE(logger, "nextYear");
	FieldsMatrix::iterator i = getPtr()->fields.begin();
	for (; i != getPtr()->fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			(*j)->renovateResources();
		}
	}
	CreaturesOnFieldVisitor *visitor = new CreatureGetOldFieldVisitor ();
	iterateCreaturesOnFields(visitor);
	delete visitor;
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
	//stateSaver->save("activitiesMade");

	// Na podstawie funkcji celu dokonaj roznmożenia
	creaturesReproducting();
	//stateSaver->save("reproductionDone");
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
	FieldsMatrix::iterator i = getPtr()->fields.begin();
	for (; i != getPtr()->fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			unsigned x = i - getPtr()->fields.begin();
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
	FieldsMatrix::iterator i = getPtr()->fields.begin();
	for (; i != getPtr()->fields.end(); i++) {
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
		delete population;
	}
	return population;
}