/*
 * Creature.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */


#include "Config.h"

#include "Field.h"
#include "Types.h"
#include "Recipe.h"
#include "Market.h"
#include "Creature.h"
#include "World.h"
#include "Population.h"
LoggerPtr Creature::logger(Logger::getLogger("creature"));

// Liczba róznych strategii wyboru aktywności
// przykłady :
// 1. Czynności o rosnącej liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 2. Czynności o malejącej  liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 3. Na zmianę zaczynając od czynności od najmniejszej liczby argumentów
#define CHOOSING_ACTIVITY_STRATEGY_COUNT 1
#define ACTIVITIES_GEN_START 1
#define ENERGY_INDEX 0

class CreatureFenotype: public GAEvalData {

public:
	UnsignedVector gainedArticlesQuants;
	UnsignedVector lostArticlesQuants;
	int fieldCoordX;
	int fieldCoordY;
	unsigned yearsOld;
	CreaturesPopulation *population;
	Field *field;

	float wallet;
	UnsignedVector articleStocks;

	CreatureFenotype(CreaturesPopulation *population, Field *field);
	virtual GAEvalData* clone() const;
	virtual void copy(const GAEvalData &src);

private:
	float performanceRatio;
	static LoggerPtr logger;
};

class CreatureActivity {
protected:
	Creature *creature;
	const Field *field;
	UnsignedVector arguments;
public:
	CreatureActivity(Creature *creature, UnsignedVector &arguments) {
		this->creature = creature;
		this->field = creature->getField();
		copy(arguments.begin(), arguments.end(), this->arguments.begin());
	}
	virtual void make() = 0;
};

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
		unsigned X, Y;
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
		x = creature->getX();
		if (creature->getY() == 0) {
			y = World::getWorld()->getHeight() - 1;
		} else {
			y--;
		}
	}
};

class GoDownActivity: public MoveActivity {
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		x = creature->getX();
		if (creature->getY() == World::getWorld()->getHeight() - 1) {
			y = 0;
		} else {
			y++;
		}
	}
};

class GoRightActivity: MoveActivity {
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		y = creature->getY();
		if (creature->getX() == World::getWorld()->getWidth() - 1) {
			x = 0;
		} else {
			x++;
		}
	}
};

class GoLeftActivity: public MoveActivity {
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		y = creature->getY();
		if (creature->getX() == 0) {
			x = World::getWorld()->getWidth() - 1;
		} else {
			x--;
		}
	}
};

class RestActivity: public Arg0CreatureActivity {
protected:
	virtual void makeActvity() {
		creature->rest();
	}
};

class Arg1CreatureActivity: public CreatureActivity {
public:
	void make() {
		assert (arguments.size() == 1);
		unsigned articleId = arguments.at(0);
		makeActvity(articleId);
	}
protected:
	virtual void makeActvity(unsigned articleId) =0;
};

class ProduceArticle: public Arg1CreatureActivity {
public:
	void makeActvity(unsigned articleId) {
		const Recipe *recipe = field->getRecipe(articleId);
		bool recipeExists = recipe != NULL;
		if (recipeExists) {
			UnsignedVector ingredients = recipe->getIngredientsVector();
			creature->produce(articleId, ingredients);
		}
	}
};

class CollectArticle: public Arg1CreatureActivity {
public:
	void makeActvity(unsigned articleId) {
		creature->collect(articleId);
	}
};

class LeaveArticle: public Arg1CreatureActivity {
public:
	void makeActvity(unsigned articleId) {
		creature->leave(articleId);
	}
};

class SellArticle: public Arg1CreatureActivity {
public:
	void makeActvity(unsigned articleId) {
		creature->sell(articleId);
	}
};

class BuyArticle: public Arg1CreatureActivity {
public:
	void makeActvity(unsigned articleId) {
		creature->buy(articleId);
	}
};

class CheckArticle: public Arg1CreatureActivity {
public:
	void makeActvity(unsigned articleId) {
		creature->check(articleId);
	}
};

float Objective(GAGenome &g) {
	Creature *c = (Creature *) &g;
	CreatureFenotype * fenotype = c->getFenotype();
	float value = 0.0;
	//World *world = World::getWorld();
	const Field *field = fenotype->field;
	// Możliwe funkcje celu
	// 1. Stosunek wartości wytworzonych produktów do wartości zużytych surowców jest największy
	// 2. Wartość wytworzonych produktów jest największa


	// wersja 1
	// Wartość wytworzonych produktów
	/*float productsValueSum = 0.0;
	 for (int i = 0; i < World::NO_OF_PRODUCTS; i++) {
	 productsValueSum += field->productPrice(i)
	 * fenotype->createdProductQuants.at(i);
	 }
	 if (productsValueSum != 0) {
	 // Wartość zużytych surowców
	 float resourcesValueSum = 0.0;
	 for (int i = 0; i < World::NO_OF_PRODUCTS; i++) {
	 resourcesValueSum += field->resourcePrice(i)
	 * fenotype->usedResourcesQuants.at(i);
	 }
	 // Stosunek wartości produktu do wartości zużycia
	 value = POSITIVE_INFINITY;
	 if (resourcesValueSum > 0)
	 value = productsValueSum / resourcesValueSum;
	 }*/
	LOG4CXX_DEBUG(Creature::logger, "Objective NOT IMPLEMENTED !!!!  : " << value);
	return value;
}

LoggerPtr CreatureFenotype::logger(Logger::getLogger("creatureFenotype"));

CreatureFenotype::CreatureFenotype(CreaturesPopulation * population,
		Field *field) {
	gainedArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	lostArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	articleStocks.assign(World::NO_OF_ARTICLES, 0);
	yearsOld = 0;
	//	previousFieldIdexes = std::pair<unsigned, unsigned>(0, 0);
	this->population = population;
	this->field = field;
}
;

GAEvalData* CreatureFenotype::clone() const {
	CreatureFenotype *cf = new CreatureFenotype(this->population, this->field);
	cf->copy(*this);
	return cf;
}

void CreatureFenotype::copy(const GAEvalData&src) {
	LOG4CXX_TRACE(logger, "CreatureFenotype::copy");
	gainedArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	lostArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	lostArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
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
	buf << *this;
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
			alleles.add(0, ZeroArgActivitiesSIZE, 1);
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
	/*JSONArray talents = creature->at(L"talents")->AsArray();
	 int talentsSize = talents.size();
	 LOG4CXX_DEBUG(logger, "talentsSize = " << talentsSize << ", h->noOfTalents() = " << h->noOfTalents());
	 assert (talentsSize == h->noOfTalents());
	 int i;
	 int geneShift = 0;
	 for (i = 0; i < talentsSize; i++) {
	 JSONArray talent = talents.at(i)->AsArray();
	 // surowiec
	 h->gene(geneShift++, talent.at(0)->AsNumber());
	 // Indeks produktu
	 h->gene(geneShift++, talent.at(1)->AsNumber());
	 // Wspolczynnik przetwarzania
	 h->gene(geneShift++, talent.at(2)->AsNumber());
	 }
	 JSONArray needs = creature->at(L"needs")->AsArray();
	 int needsSize = needs.size();
	 assert(needsSize == h->noOfNeeds());
	 for (i = 0; i < needsSize; i++) {
	 JSONArray need = needs.at(i)->AsArray();
	 // Produkt
	 h->gene(geneShift++, need.at(0)->AsNumber());
	 // Potrzeba
	 h->gene(geneShift++, need.at(1)->AsNumber());
	 }
	 JSONArray directions = creature->at(L"directions")->AsArray();
	 int directionsSize = directions.size();
	 for (i = 0; i < directionsSize; i++) {
	 // Kierunek
	 h->gene(geneShift++, directions.at(i)->AsNumber());
	 }*/
}

Creature::Creature(CreaturesPopulation *population, Field * field,
		JSONObject &creature) :
	GARealGenome(Creature::allelesDefinition(population), Objective) {
	// Wypełnij tymczasową strukturę z definicją osobnika
	userData(&creature);
	evalData(CreatureFenotype(population, field));
	initializer(JSONInitializer);
	initialize();
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
	getFenotype()->population = to;
}

float Creature::randBetweenAndStepped(float min, float max, float step) {
	float zeroOne = (random() / (float) RAND_MAX);
	float randBetween = min + zeroOne * (max - min);
	int intVal = randBetween / step;
	return intVal * step;
}

Creature* Creature::clone(GAGenome::CloneMethod flag) {
	return new Creature(*this);
}

void Creature::copy(const Creature&) {
	GARealGenome::copy(*this);
}

Creature::~Creature() {
	delete this->evd;
}

void Creature::doAllActivities() {
	/*CreatureActivityList activities = getCreatureActivities();
	CreatureActivityList::iterator activity = activities.begin();
	for (; activity != activities.end(); activity++) {
		(*activity)->make();
		delete (*activity);
	}*/
}

bool Creature::hasEnergy() const {
	return getFenotype()->articleStocks.at(ENERGY_INDEX) > 0;
}
void Creature::rest() {
	getFenotype()->articleStocks.at(ENERGY_INDEX)++;
}

bool Creature::produce(unsigned articleId,
		 std::vector<unsigned> ingredients) {
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
	}
	return produced;
}

bool Creature::collect(unsigned articleId) {
	bool canCollect = getField()->tryTakeArticle(articleId);
	if (canCollect) {
		getFenotype()->gainedArticlesQuants.at(articleId) += 1;
	}
	return canCollect;
}

bool Creature::leave(unsigned articleId) {
	CreatureFenotype *fenotype = getFenotype();
	bool canLeave = fenotype->gainedArticlesQuants.at(articleId) >= 1.0;
	if (canLeave) {
		fenotype->gainedArticlesQuants.at(articleId) -= 1.0;
		fenotype->field->putArticle(articleId);
	}
	return canLeave;
}

bool Creature::sell(unsigned articleId) {
	CreatureFenotype *fenotype = getFenotype();
	bool canSell = fenotype->articleStocks.at(articleId) >= 1.0;
	if (canSell) {
		float moneyEarned = fenotype->field->getMarket()->buyArticleFromClient(
				getId(), articleId);
		fenotype->wallet += moneyEarned;
	}
	return canSell;
}

bool Creature::buy(unsigned articleId) {
	CreatureFenotype *fenotype = getFenotype();
	bool bought = fenotype->field->getMarket()->sellArticleToClient(getId(),
			articleId, fenotype->wallet);
	return bought;
}

bool Creature::check(unsigned articleId) {
	CreatureFenotype *fenotype = getFenotype();
	fenotype->field->getMarket()->articleSellPrice(getId(), articleId);
	return true;
}

unsigned Creature::getX() const {
	return getFenotype()->fieldCoordX;
}

unsigned Creature::getY() const {
	return getFenotype()->fieldCoordY;
}

CreatureActivity *Creature::createActivity(unsigned activityGenIndex,
		unsigned parametersCount) {
	UnsignedVector parameters;
	CreatureActivity *activity;

	for (unsigned i = 0; i < parametersCount; i++) {
		parameters.push_back(gene(i + 1));
	}
	switch (parametersCount) {
	case 0:
		ZeroArgActivities zeroArgActivity = (ZeroArgActivities) gene(
				activityGenIndex);
		switch (zeroArgActivity) {
		case GoUp:
			activity = new GoUpActivity(this, parameters);
			break;
		case 1:
		case 2:
		default:
			assert (false);
		}
		unsigned activityTableIndex = gene(activityGenIndex);

	}
	return activity;
}

CreatureActivityList Creature::getCreatureActivities() {
	CreatureActivityList activities;
	ActivitiesStrategy activitiesStrategy = getActiviviesStrategy();
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
	bool moved = hasEnergy();
	if (moved) {
		Field *newField = World::getWorld()->fields.at(x).at(y);
		CreaturesPopulation *population = getFenotype()->population;
		CreaturesPopulation *newPopulation =
				World::getWorld()->findOrCreatePopulationOnField(population, x,
						y);
		getFenotype()->field = newField;
		getFenotype()->population = newPopulation;
		changePopulation(population, newPopulation);
		getFenotype()->fieldCoordX = x;
		getFenotype()->fieldCoordY = y;
		getFenotype()->articleStocks.at(ENERGY_INDEX) -= 1;
	}
	return moved;
}

