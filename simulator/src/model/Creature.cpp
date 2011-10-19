/*
 * Creature.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */
#include <algorithm>
#include "Config.h"
#include "Field.h"
#include "Recipe.h"
#include "Market.h"
#include "Creature.h"
#include "World.h"
#include "Article.h"
#include "Population.h"
LoggerPtr Creature::logger(Logger::getLogger("creature"));
unsigned Creature::createuresId = 0;

// Liczba róznych strategii wyboru aktywności
// przykłady :
// 1. Czynności o rosnącej liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 2. Czynności o malejącej  liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 3. Na zmianę zaczynając od czynności od najmniejszej liczby argumentów
#define CHOOSING_ACTIVITY_STRATEGY_COUNT 1
#define ACTIVITIES_STRATEGY_GEN 0
#define ACTIVITIES_GEN_START 1
#define ENERGY_INDEX 0

std::vector<std::string> Creature::ZeroArgActivitiesNames;
std::vector<std::string> Creature::OneArgActivitiesNames;
std::vector<std::string> Creature::TwoArgActivitiesNames;

void Creature::initNames() {
	const char * za[] = { "goUp", "goDown", "goLeft", "goRight", "rest" };
	const char * oa[] = { "produce", "collect", "leave", "sell", "buy",
			"check", "eat" };
	const char * ta[] = { "exchange" };

	int i;
	for (i = 0; i < ZeroArgActivitiesSIZE; i++) {
		ZeroArgActivitiesNames.push_back(za[i]);
	}
	for (i = 0; i < OneArgActivitiesSIZE; i++) {
		OneArgActivitiesNames.push_back(oa[i]);
	}
	for (i = 0; i < TwoArgActivitiesSIZE; i++) {
		TwoArgActivitiesNames.push_back(ta[i]);
	}
}

unsigned Creature::get0ArgActivityIndex(std::string activityName) {
	std::vector<std::string>::iterator activityIterator = find(
			ZeroArgActivitiesNames.begin(), ZeroArgActivitiesNames.end(),
			activityName);
	LOG4CXX_DEBUG(logger, "Searching in vector: " << activityName << " ... iterator: " << *activityIterator);
	assert(activityIterator != ZeroArgActivitiesNames.end());
	return activityIterator - ZeroArgActivitiesNames.begin();
}

unsigned Creature::get1ArgActivityIndex(std::string activityName) {
	std::vector<std::string>::iterator activityIterator = find(
			OneArgActivitiesNames.begin(), OneArgActivitiesNames.end(),
			activityName);
	LOG4CXX_DEBUG(logger, "Searching in vector: " << activityName << " ... iterator: " << *activityIterator);
	assert(activityIterator != OneArgActivitiesNames.end());
	return activityIterator - OneArgActivitiesNames.begin();
}
unsigned Creature::get2ArgActivityIndex(std::string activityName) {
	std::vector<std::string>::iterator activityIterator = find(
			TwoArgActivitiesNames.begin(), TwoArgActivitiesNames.end(),
			activityName);
	LOG4CXX_DEBUG(logger, "Searching in vector: " << activityName << " ... iterator: " << *activityIterator);
	assert(activityIterator != TwoArgActivitiesNames.end());
	return activityIterator - TwoArgActivitiesNames.begin();
}

class CreatureFenotype: public GAEvalData {

public:
	unsigned fieldCoordX;
	unsigned fieldCoordY;
	unsigned yearsOld;
	std::string id;
	CreaturesPopulation *population;
	Field *field;

	float wallet;
	UnsignedVector articleStocks;
	IntVector articleQuantsChange;
	CreatureFenotype(CreaturesPopulation *population, Field *field, unsigned x,
			unsigned y);
	virtual GAEvalData* clone() const;
	virtual void copy(const GAEvalData &src);

private:
	float performanceRatio;
	static LoggerPtr logger;
};

CreatureActivity::CreatureActivity(Creature *creature,
		UnsignedVector &arguments) {
	this->creature = creature;
	this->field = creature->getField();
	this ->arguments.insert(this->arguments.begin(), arguments.begin(),
			arguments.end());
	this->creature->evaluate(gaTrue);

}

class Arg0CreatureActivity: public CreatureActivity {
public:
	Arg0CreatureActivity(Creature *creature, UnsignedVector &arguments) :
		CreatureActivity(creature, arguments) {
	}
	void make() {
		assert (arguments.size() == 0);
		makeActvity();
	}
protected:
	virtual void makeActvity() =0;
};

class MoveActivity: public Arg0CreatureActivity {
public:
	MoveActivity(Creature *creature, UnsignedVector &arguments) :
		Arg0CreatureActivity(creature, arguments) {
	}
protected:
	virtual void makeActvity() {
		unsigned X = creature->getX();
		unsigned Y = creature->getY();
		calculateNewCoordinates(X, Y);
		creature->move(X, Y);
	}

	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) = 0;
};

class GoUpActivity: public MoveActivity {
public:
	GoUpActivity(Creature *creature, UnsignedVector &arguments) :
		MoveActivity(creature, arguments) {
	}
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		if (y == 0) {
			y = World::HEIGHT - 1;
		} else {
			y--;
		}
	}
};

class GoDownActivity: public MoveActivity {
public:
	GoDownActivity(Creature *creature, UnsignedVector &arguments) :
		MoveActivity(creature, arguments) {
	}
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		if (y == World::HEIGHT - 1) {
			y = 0;
		} else {
			y++;
		}
	}
};

class GoRightActivity: public MoveActivity {
public:
	GoRightActivity(Creature *creature, UnsignedVector &arguments) :
		MoveActivity(creature, arguments) {
	}
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		if (x == World::WIDTH - 1) {
			x = 0;
		} else {
			x++;
		}
	}
};

class GoLeftActivity: public MoveActivity {
public:
	GoLeftActivity(Creature *creature, UnsignedVector &arguments) :
		MoveActivity(creature, arguments) {
	}
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		if (x == 0) {
			x = World::WIDTH - 1;
		} else {
			x--;
		}
	}
};

class RestActivity: public Arg0CreatureActivity {
public:
	RestActivity(Creature *creature, UnsignedVector &arguments) :
		Arg0CreatureActivity(creature, arguments) {
	}
protected:
	virtual void makeActvity() {
		creature->rest();
	}
};

class Arg1CreatureActivity: public CreatureActivity {
public:
	Arg1CreatureActivity(Creature *creature, UnsignedVector &arguments) :
		CreatureActivity(creature, arguments) {
	}

	void make() {
		assert (arguments.size() == 1);
		unsigned articleId = arguments.at(0);
		makeActvity(articleId);
	}
protected:
	virtual void makeActvity(unsigned articleId) =0;
};

class ProduceArticleActivity: public Arg1CreatureActivity {
public:
	ProduceArticleActivity(Creature *creature, UnsignedVector &arguments) :
		Arg1CreatureActivity(creature, arguments) {
	}
protected:
	void makeActvity(unsigned articleId) {
		const Recipe *recipe = field->getRecipe(articleId);
		bool recipeExists = recipe != NULL;
		if (recipeExists) {
			UnsignedVector ingredients = recipe->getIngredientsVector();
			creature->produce(articleId, ingredients);
		}
	}
};

class CollectArticleActivity: public Arg1CreatureActivity {
public:
	CollectArticleActivity(Creature *creature, UnsignedVector &arguments) :
		Arg1CreatureActivity(creature, arguments) {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->collect(articleId);
	}
};

class LeaveArticleActivity: public Arg1CreatureActivity {
public:
	LeaveArticleActivity(Creature *creature, UnsignedVector &arguments) :
		Arg1CreatureActivity(creature, arguments) {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->leave(articleId);
	}
};

class SellArticleActivity: public Arg1CreatureActivity {
public:
	SellArticleActivity(Creature *creature, UnsignedVector &arguments) :
		Arg1CreatureActivity(creature, arguments) {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->sell(articleId);
	}
};

class BuyArticleActivity: public Arg1CreatureActivity {
public:
	BuyArticleActivity(Creature *creature, UnsignedVector &arguments) :
		Arg1CreatureActivity(creature, arguments) {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->buy(articleId);
	}
};

class CheckArticleActivity: public Arg1CreatureActivity {
public:
	CheckArticleActivity(Creature *creature, UnsignedVector &arguments) :
		Arg1CreatureActivity(creature, arguments) {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->check(articleId);
	}
};

class EatArticleActivity: public Arg1CreatureActivity {
public:
	EatArticleActivity(Creature *creature, UnsignedVector &arguments) :
		Arg1CreatureActivity(creature, arguments) {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->eat(articleId);
	}
};
float Objective(GAGenome &g) {
	Creature *c = (Creature *) &g;
	CreatureFenotype * fenotype = c->getFenotype();
	float value = 0.0;
	//World *world = World::getWorld();
	Field *field = fenotype->field;
	// Możliwe funkcje celu
	// 1. Stosunek wartości wytworzonych produktów do wartości zużytych surowców jest największy
	// 2. Wartość wytworzonych produktów jest największa


	// wersja 1
	// Wartość wytworzonych produktów
	float articlesValueSum = 0.0;
	for (unsigned i = 0; i < World::NO_OF_ARTICLES; i++) {
		articlesValueSum += field->getMarket()->articleBuyPrice(i)
				* fenotype->articleStocks.at(i);

	}
	value = //articlesValueSum/100.0 +
			fenotype->wallet;
	if (!c->getId().empty()) {
		LOG4CXX_DEBUG(Creature::logger, "creature " <<c->getId() << " objective = " << value);
	}
	return value;
}

LoggerPtr CreatureFenotype::logger(Logger::getLogger("creatureFenotype"));

CreatureFenotype::CreatureFenotype(CreaturesPopulation * population,
		Field *field, unsigned x, unsigned y) {
	LOG4CXX_TRACE(logger, "CreatureFenotype::CreatureFenotype");
	fieldCoordX = x;
	fieldCoordY = y;
	articleQuantsChange.assign(World::NO_OF_ARTICLES, 0);
	articleStocks.assign(World::NO_OF_ARTICLES, 0);
	yearsOld = 0;
	//	previousFieldIdexes = std::pair<unsigned, unsigned>(0, 0);
	this->population = population;
	this->field = field;
}
;

GAEvalData* CreatureFenotype::clone() const {
	LOG4CXX_TRACE(logger, "CreatureFenotype::clone");
	CreatureFenotype *cf = new CreatureFenotype(this->population, this->field,
			this->fieldCoordX, this->fieldCoordY);
	cf->copy(*this);
	return cf;
}

void CreatureFenotype::copy(const GAEvalData&src) {
	LOG4CXX_TRACE(logger, "CreatureFenotype::copy");
	articleQuantsChange.assign(World::NO_OF_ARTICLES, 0);
	yearsOld = 0;// (((CreatureFenotype &) src).yearsOld);
	population = (((CreatureFenotype &) src).population);
	field = (((CreatureFenotype &) src).field);
	wallet = 0;
}

GARealAlleleSetArray Creature::alleles;

/*int Creature::noOfTalents() {
 return getFenotype()->population->getCreaturesTalentsCount();
 }

 int Creature::noOfNeeds() {
 return getFenotype()->population->getCreaturesNeedsCount();
 }*/

std::string Creature::genomeStr() const {
	std::stringstream buf;
	unsigned i;
	const Creature *h = this;
	int geneShift = 1; // pomijam strategie
	// kodowanie czynnosci 0 argumentowych
	unsigned activitiesSize =
			h->getFenotype()->population->get0ArgActivitiesRoom();
	for (i = 0; i < activitiesSize; i++) {
		buf << ZeroArgActivitiesNames[(int) h->gene(geneShift++)] << " ";
	}
	// kodowanie czynnosci 1 argumentowych
	activitiesSize = h->getFenotype()->population->get1ArgActivitiesRoom();
	for (i = 0; i < activitiesSize; i++) {
		// czynnosc
		buf << OneArgActivitiesNames[(int) h->gene(geneShift++)] << "(";
		// argument
		buf << World::ARTICLES[h->gene(geneShift++)] << ") ";
	}
	// kodowanie czynnosci 1 argumentowych
	activitiesSize = h->getFenotype()->population->get2ArgActivitiesRoom();
	for (i = 0; i < activitiesSize; i++) {
		// czynnosc
		buf << TwoArgActivitiesNames[(int) h->gene(geneShift++)] << "(";
		// argument 1
		buf << World::ARTICLES[h->gene(geneShift++)] << ",";
		// argument 2
		buf << World::ARTICLES[h->gene(geneShift++)] << ")";
	}
	return buf.str();
}

GARealAlleleSetArray Creature::allelesDefinition(
		const CreaturesPopulation *population) {
	if (alleles.size() == 0) {
		// Strategia
		alleles.add(0, CHOOSING_ACTIVITY_STRATEGY_COUNT - 1, 1);
		unsigned i = 0;
		// Kodowanie czynności bezargumentowych (np. ruch w lewo)
		for (; i < population->get0ArgActivitiesRoom(); i++) {
			alleles.add(0, ZeroArgActivitiesSIZE - 1, 1);
		}
		// Kodowanie czynności jednoargumentowych (np. kupno towaru)
		for (i = 0; i < population->get1ArgActivitiesRoom(); i++) {
			// Indeks czynności
			alleles.add(0, OneArgActivitiesSIZE - 1, 1);
			// Argument - artykuł
			alleles.add(0, World::NO_OF_ARTICLES - 1, 1);
		}
		// Kodowanie czynności dwuargumentowych (np. wymiana towaru za towar)
		for (i = 0; i < population->get2ArgActivitiesRoom(); i++) {
			// Indeks czynności
			alleles.add(0, TwoArgActivitiesSIZE - 1, 1);
			// Argument 1 - artykuł
			alleles.add(0, World::NO_OF_ARTICLES - 1, 1);
			// Argument 2 - artykuł
			alleles.add(0, World::NO_OF_ARTICLES - 1, 1);
		}
	}
	return alleles;
}

/*void Creature::RandomInitializer(GAGenome &g) {
 Creature *h = (Creature*) &g;
 int i = 0;
 unsigned j = 0;
 for (; i < h->noOfTalents(); i++) {
 // Indeks zasobu
 h->gene(j++, randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1));
 // Indeks produktu
 h->gene(j++, randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1));
 // Wspolczynnik przetwarzania
 h->gene(j++, randBetweenAndStepped(TALENTS_F_BOUNDS[0],
 TALENTS_F_BOUNDS[1], TALENTS_F_BOUNDS[2]));
 }
 // Potrzeby
 for (i = 0; i < h->noOfNeeds(); i++) {
 // Indeks produktu
 h->gene(j++, randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1));
 // Wspolczynnik przetwarzania
 h->gene(j++, randBetweenAndStepped(NEEDS_BOUNDS[0], NEEDS_BOUNDS[1],
 NEEDS_BOUNDS[2]));
 }
 // Droga do przebycia
 i = 0;
 for (; i < MAX_POINT_ON_ROAD; i++) {
 // 4 kierunki
 h->gene(j++, randBetweenAndStepped(DIRECTION_BOUNDS[0],
 DIRECTION_BOUNDS[1], DIRECTION_BOUNDS[2]));
 }
 }*/

void Creature::DoNothingInitializer(GAGenome &g) {

}

/*Creature::Creature(const CreaturesPopulation *population, const Field * field) :
 GARealGenome(Creature::allelesDefinition(population), Objective) {
 LOG4CXX_TRACE(logger, "Creature::Creature");
 evalData(CreatureFenotype(population, field));
 initializer( RandomInitializer);
 initialize();
 LOG4CXX_DEBUG(logger, "genome : " << *this);
 }*/

void Creature::JSONInitializer(GAGenome &g) {
	Creature *h = (Creature *) &g;
	JSONObject *creature = (JSONObject *) h->userData();
	int geneShift = 0;
	// strategia
	h->gene(geneShift++, creature->at(L"activitiesStrategy")->AsNumber());
	unsigned i;
	// kodowanie czynnosci 0 argumentowych
	JSONArray activities = creature->at(L"0ArgActivity")->AsArray();
	unsigned activitiesSize = activities.size();
	assert (activitiesSize == h->getFenotype()->population->get0ArgActivitiesRoom());
	for (i = 0; i < activitiesSize; i++) {
		// czynnosc
		std::string activityName = wstring2string(activities.at(i)->AsString());
		int activityIndex = get0ArgActivityIndex(activityName);
		h->gene(geneShift++, activityIndex);
	}
	// kodowanie czynnosci 1 argumentowych
	activities = creature->at(L"1ArgActivity")->AsArray();
	activitiesSize = activities.size();
	assert (activitiesSize == h->getFenotype()->population->get1ArgActivitiesRoom());
	for (i = 0; i < activitiesSize; i++) {
		JSONArray activity = activities.at(i)->AsArray();
		// czynnosc
		std::string activityName = wstring2string(activity.at(0)->AsString());
		unsigned activityIndex = get1ArgActivityIndex(activityName);
		h->gene(geneShift++, activityIndex);
		// argument
		unsigned articleIndex = World::getArticleIndex(
				activity.at(1)->AsString());
		h->gene(geneShift++, articleIndex);

	}
	// kodowanie czynnosci 1 argumentowych
	activities = creature->at(L"2ArgActivity")->AsArray();
	activitiesSize = activities.size();
	assert (activitiesSize == h->getFenotype()->population->get2ArgActivitiesRoom());
	for (i = 0; i < activitiesSize; i++) {
		JSONArray activity = activities.at(i)->AsArray();
		// czynnosc
		std::string activityName = wstring2string(activity.at(0)->AsString());
		unsigned activityIndex = get1ArgActivityIndex(activityName);
		h->gene(geneShift++, activityIndex);
		// argument 1
		unsigned articleIndex1 = World::getArticleIndex(
				activity.at(1)->AsString());
		h->gene(geneShift++, articleIndex1);
		// argument 2
		unsigned articleIndex2 = World::getArticleIndex(
				activity.at(2)->AsString());
		h->gene(geneShift++, articleIndex2);
	}
}

Creature::Creature(CreaturesPopulation *population, Field * field,
		JSONObject &creature, unsigned x, unsigned y) :
	GARealGenome(Creature::allelesDefinition(population), Objective) {
	LOG4CXX_TRACE(logger, "Creature::Creature(CreaturesPopulation *population, Field * field, JSONObject &creature, unsigned x, unsigned y)");
	// Wypełnij tymczasową strukturę z definicją osobnika
	userData(&creature);
	evalData(CreatureFenotype(population, field, x, y));
	initializer(JSONInitializer);
	initialize();
	std::stringstream idBuf;
	idBuf << Creature::createuresId++;
	getFenotype()->id = idBuf.str();
	// Można już wyczyścić dane
	userData(NULL);
	initializer(DoNothingInitializer);
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

/*void Creature::step() {
 CreatureFenotype *fenotype = getFenotype();
 fenotype->createdProductQuants.assign(World::NO_OF_PRODUCTS, 0);
 fenotype->usedResourcesQuants.assign(World::NO_OF_RESOURCES, 0);
 this->_evaluated = gaFalse;
 fenotype->yearsOld++;
 //fenotype->objectiveValue = 0;
 }*/

void Creature::changePopulation(CreaturesPopulation *from,
		CreaturesPopulation *to) {
	assert (getFenotype()->population == from);
	from->remove(this);
	to->add(this);
}

/*
 float Creature::randBetweenAndStepped(float min, float max, float step) {
 float zeroOne = (random() / (float) RAND_MAX);
 float randBetween = min + zeroOne * (max - min);
 int intVal = randBetween / step;
 return intVal * step;
 }
 */
Creature::Creature(const Creature &creature) :
	GARealGenome(creature) {
	LOG4CXX_TRACE(logger, "Creature::Creature(const Creature &creature)");
	/*evalData(CreatureFenotype(creature.getFenotype()->population,
	 creature.getFenotype()->field, creature.getFenotype()->fieldCoordX,
	 creature.getFenotype()->fieldCoordY));
	 initializer(DoNothingInitializer);
	 initialize();
	 */
	std::stringstream idBuf;
	idBuf << Creature::createuresId++;
	evalData(CreatureFenotype(creature.getFenotype()->population,
			creature.getFenotype()->field, creature.getFenotype()->fieldCoordX,
			creature.getFenotype()->fieldCoordY));
	getFenotype()->id = idBuf.str();
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

GAGenome* Creature::clone(GAGenome::CloneMethod flag) {
	LOG4CXX_TRACE(logger, "clone");
	return new Creature(*this);
}

void Creature::copy(const GAGenome &g) {
	LOG4CXX_TRACE(logger, "copy");
	GARealGenome::copy(g);
	//getFenotype()->copy()
}

Creature::~Creature() {
	LOG4CXX_TRACE(logger, "Creature::~Creature");
	delete this->evd;
}

void Creature::doAllActivities() {
	CreatureActivityList activities = getCreatureActivities();
	CreatureActivityList::iterator activity = activities.begin();
	for (; activity != activities.end(); activity++) {
		(*activity)->make();
		delete (*activity);
	}
}

bool Creature::hasEnergy() const {
	return getFenotype()->articleStocks.at(ENERGY_INDEX) > 0;
}
void Creature::rest() {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " resting " );
	getFenotype()->articleStocks.at(ENERGY_INDEX)++;
	getFenotype()->articleQuantsChange.at(ENERGY_INDEX)++;
}

bool Creature::produce(unsigned articleId, std::vector<unsigned> ingredients) {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " producing " << World::ARTICLES.at(articleId));

	bool produced = true;
	assert (getFenotype()->articleStocks.size() == ingredients.size());
	UnsignedVector::const_iterator i = ingredients.begin();
	UnsignedVector::iterator s = getFenotype()->articleStocks.begin();
	for (; produced && i != ingredients.end(); s++, i++) {
		if (*i > *s) {
			produced = false;
		}
	}
	if (produced) {
		i = ingredients.begin();
		s = getFenotype()->articleStocks.begin();
		for (; i != ingredients.end(); s++, i++) {
			*s -= *i;
		}
		getFenotype()->articleStocks.at(articleId) += 1;
		getFenotype()->articleQuantsChange.at(articleId)++;
	}
	return produced;
}

bool Creature::collect(unsigned articleId) {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " collecting " << World::ARTICLES.at(articleId));
	bool canCollect = getField()->tryTakeArticle(articleId);
	if (canCollect) {
		getFenotype()->articleStocks.at(articleId) += 1;
		getFenotype()->articleQuantsChange.at(articleId) += 1;
	}
	return canCollect;
}

bool Creature::leave(unsigned articleId) {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " leaving " << World::ARTICLES.at(articleId));
	CreatureFenotype *fenotype = getFenotype();
	bool canLeave = fenotype->articleStocks.at(articleId) >= 1.0;
	if (canLeave) {
		getFenotype()->articleStocks.at(articleId) -= 1;
		fenotype->articleQuantsChange.at(articleId) -= 1.0;
		fenotype->field->putArticle(articleId);
	}
	return canLeave;
}

bool Creature::sell(unsigned articleId) {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " selling " << World::ARTICLES.at(articleId));
	CreatureFenotype *fenotype = getFenotype();
	bool canSell = fenotype->articleStocks.at(articleId) >= 1.0;
	if (canSell) {
		float moneyEarned = fenotype->field->getMarket()->buyArticleFromClient(
				getId(), articleId);
		fenotype->wallet += moneyEarned;
		fenotype->articleStocks.at(articleId) -= 1;
		fenotype->articleQuantsChange.at(articleId) -= 1;
	}
	return canSell;
}

bool Creature::buy(unsigned articleId) {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " buying " << World::ARTICLES.at(articleId));
	CreatureFenotype *fenotype = getFenotype();
	bool bought = fenotype->field->getMarket()->sellArticleToClient(getId(),
			articleId, fenotype->wallet);
	if (bought) {
		fenotype->articleStocks.at(articleId) += 1;
		fenotype->articleQuantsChange.at(articleId) += 1;
	}
	return bought;
}

bool Creature::check(unsigned articleId) {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " checking " << World::ARTICLES.at(articleId));
	CreatureFenotype *fenotype = getFenotype();
	fenotype->field->getMarket()->articleSellPrice(getId(), articleId);
	return true;
}

bool Creature::eat(unsigned articleId) {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " eating " << World::ARTICLES.at(articleId));
	CreatureFenotype *fenotype = getFenotype();
	bool eaten = fenotype->articleStocks.at(articleId) > 0;
	if (fenotype->articleStocks.at(articleId) > 0) {
		fenotype->articleStocks.at(articleId) -= 1;
		fenotype->articleQuantsChange.at(articleId) -= 1;
		int lessOrMore = World::articles.at(articleId)->isFood() ? 1 : -1;
		fenotype->articleStocks.at(ENERGY_INDEX) += lessOrMore;
		fenotype ->articleQuantsChange.at(ENERGY_INDEX) += lessOrMore;
	}
	return eaten;
}

Field *Creature::getField() {
	return getFenotype()->field;
}
unsigned Creature::getX() const {
	return getFenotype()->fieldCoordX;
}

unsigned Creature::getY() const {
	return getFenotype()->fieldCoordY;
}

std::string Creature::getId() const {
	return getFenotype()->id;
}

unsigned Creature::getAge() const {
	return getFenotype()->yearsOld;
}

float Creature::getWallet() const {
	return getFenotype()->wallet;
}

unsigned Creature::getArticleStock(unsigned articleId) const {
	return getFenotype()->articleStocks.at(articleId);
}

int Creature::getArticleQuantChange(unsigned articleId) const {
	return getFenotype()->articleQuantsChange.at(articleId);
}

//gainedArticlesQuants

CreatureActivity *Creature::createActivity(unsigned activityGenIndex,
		unsigned parametersCount) {
	UnsignedVector parameters;
	CreatureActivity *activity;

	for (unsigned i = 0; i < parametersCount; i++) {
		parameters.push_back(gene(activityGenIndex + i + 1));
	}
	switch (parametersCount) {
	case 0: {
		ZeroArgActivities zeroArgActivity = (ZeroArgActivities) gene(
				activityGenIndex);
		switch (zeroArgActivity) {
		case GoUp:
			activity = new GoUpActivity(this, parameters);
			break;
		case GoDown:
			activity = new GoDownActivity(this, parameters);
			break;
		case GoLeft:
			activity = new GoLeftActivity(this, parameters);
			break;
		case GoRight:
			activity = new GoRightActivity(this, parameters);
			break;
		case Rest:
			activity = new RestActivity(this, parameters);
			break;
		default:
			assert (false);
		}
		break;
	}
		//unsigned activityTableIndex = gene(activityGenIndex);
	case 1: {
		OneArgActivities oneArgActivity = (OneArgActivities) gene(
				activityGenIndex);
		switch (oneArgActivity) {
		case ProduceArticle:
			activity = new ProduceArticleActivity(this, parameters);
			break;
		case CollectArticle:
			activity = new CollectArticleActivity(this, parameters);
			break;
		case LeaveArticle:
			activity = new LeaveArticleActivity(this, parameters);
			break;
		case SellArticle:
			activity = new SellArticleActivity(this, parameters);
			break;
		case BuyArticle:
			activity = new BuyArticleActivity(this, parameters);
			break;
		case CheckArticle:
			activity = new CheckArticleActivity(this, parameters);
			break;
		case EatArticle:
			activity = new EatArticleActivity(this, parameters);
			break;
		default:
			assert (false);
		}
		break;
	}
	}
	return activity;
}

CreatureActivityList Creature::getCreatureActivities() {
	CreatureActivityList activities;
	ActivitiesStrategy activitiesStrategy = (ActivitiesStrategy) gene(
			ACTIVITIES_STRATEGY_GEN);//getActiviviesStrategy();
	switch (activitiesStrategy) {
	case AscengingArgumentCountOrder: {
		CreatureActivity *activity = NULL;
		int activityGenIndex = ACTIVITIES_GEN_START;
		int first0ArgActivityGenIndex = ACTIVITIES_GEN_START;
		int numberOf0ArgActivities =
				getFenotype()->population->get0ArgActivitiesRoom();
		int numberOf1ArgActivities =
				getFenotype()->population->get1ArgActivitiesRoom();
		int numberOf2ArgActivities =
				getFenotype()->population->get2ArgActivitiesRoom();
		int stopIndex = ACTIVITIES_GEN_START + numberOf0ArgActivities;
		for (; activityGenIndex < stopIndex; activityGenIndex++) {
			activity = createActivity(activityGenIndex, 0);
			activities.push_back(activity);
		}
		stopIndex += numberOf1ArgActivities * 2;
		for (; activityGenIndex < stopIndex;) {
			activity = createActivity(activityGenIndex, 1);
			activities.push_back(activity);
			activityGenIndex += 2;
		}
		stopIndex += numberOf2ArgActivities * 3;
		for (; activityGenIndex < stopIndex;) {
			activity = createActivity(activityGenIndex, 2);
			activities.push_back(activity);
			activityGenIndex += 3;
		}
		break;
	}
	}
	return activities;
}

bool Creature::move(unsigned x, unsigned y) {
	LOG4CXX_DEBUG(logger, "craeture " << getId() << " moving to " << x << "x" << y );
	bool anotherField = (x != getFenotype()->fieldCoordX || y
			!= getFenotype()->fieldCoordY);
	bool moved = anotherField && hasEnergy();
	if (moved) {
		Field *newField = World::getWorld()->fields.at(x).at(y);
		CreaturesPopulation *population = getFenotype()->population;
		CreaturesPopulation *newPopulation =
				World::getWorld()->findOrCreatePopulation(population, x, y);
		changePopulation(population, newPopulation);
		getFenotype()->field = newField;
		getFenotype()->population = newPopulation;
		getFenotype()->fieldCoordX = x;
		getFenotype()->fieldCoordY = y;
		getFenotype()->articleStocks.at(ENERGY_INDEX) -= 1;
		getFenotype()->articleQuantsChange.at(ENERGY_INDEX) -= 1;
	}
	return moved;
}

