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
#include "Activity.h"

LoggerPtr Creature::logger(Logger::getLogger("creature"));
unsigned Creature::createuresId = 0;

// Liczba róznych strategii wyboru aktywności
// przykłady :
// 1. Czynności o rosnącej liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 2. Czynności o malejącej  liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 3. Na zmianę zaczynając od czynności od najmniejszej liczby argumentów
#define ENERGY_INDEX 0

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
	int geneShift = 0;
	// kodowanie czynnosci
	unsigned activitiesSize =
			h->getFenotype()->population->getCreatureActivitiesCount();
	for (i = 0; i < activitiesSize; i++) {
		int activity = h->gene(geneShift++);
		int argument = h->gene(geneShift++);
		LOG4CXX_DEBUG(logger, getId() << " -> activity:" << activity << ", argument: " << argument);
		buf << Activity::getName(activity);
		if (Activity::hasArgument(activity)) {
			// argument
			buf << "(" << World::ARTICLES[argument] << ") ";
		} else {
			buf << " ";
		}

	}
	return buf.str();
}

GARealAlleleSetArray Creature::allelesDefinition(
		const CreaturesPopulation *population) {
	if (alleles.size() == 0) {
		unsigned i = 0;
		// Kodowanie czynności jednoargumentowych
		for (i = 0; i < population->getCreatureActivitiesCount(); i++) {
			// Indeks czynności
			alleles.add(0, Activity::COUNT - 1, 1);
			// Argument - artykuł
			alleles.add(0, World::NO_OF_ARTICLES - 1, 1);
		}
	}
	return alleles;
}

void Creature::RandomInitializer(GAGenome &g) {
	Creature *h = (Creature*) &g;
	int i = 0;
	unsigned j = 0;
	for (; i < h->getActivitiesCount(); i++) {
		// Indeks czynnosci
		h->gene(j++, randBetweenAndStepped(0, Activity::COUNT - 1, 1));
		// Indeks produktu
		h->gene(j++, randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1));
	}
}

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
	unsigned i;
	// kodowanie czynnosci 1 argumentowych
	JSONArray activities = creature->at(L"activities")->AsArray();
	unsigned activitiesSize = activities.size();
	assert (activitiesSize == h->getFenotype()->population->getCreatureActivitiesCount());
	for (i = 0; i < activitiesSize; i++) {
		JSONObject activity = activities.at(i)->AsObject();
		// czynnosc
		std::string activityName = wstring2string(activity.begin()->first);
		unsigned activityIndex = Activity::getIndex(activityName);
		h->gene(geneShift++, activityIndex);
		unsigned articleIndex = 0;
		if (activity.begin()->second->IsNull()) {
			articleIndex = rand() % World::ARTICLES.size();
		} else {
			// argument
			articleIndex = World::getArticleIndex(
					activity.begin()->second->AsString());

		}
		h->gene(geneShift++, articleIndex);
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

Creature::Creature(CreaturesPopulation *population, Field * field, unsigned x,
		unsigned y) :
	GARealGenome(Creature::allelesDefinition(population), Objective) {
	LOG4CXX_TRACE(logger, "Creature::Creature(CreaturesPopulation *population, Field * field, unsigned x, unsigned y)");
	// Wypełnij tymczasową strukturę z definicją osobnika
	evalData(CreatureFenotype(population, field, x, y));
	initializer(RandomInitializer);
	initialize();
	std::stringstream idBuf;
	idBuf << Creature::createuresId++;
	getFenotype()->id = idBuf.str();
	initializer(DoNothingInitializer);
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

void Creature::changePopulation(CreaturesPopulation *from,
		CreaturesPopulation *to) {
	assert (getFenotype()->population == from);
	from->remove(this);
	to->add(this);
}

Creature::Creature(const Creature &creature) :
	GARealGenome(creature) {
	LOG4CXX_TRACE(logger, "Creature::Creature(const Creature &creature)");
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
}

Creature::~Creature() {
	LOG4CXX_TRACE(logger, "Creature::~Creature");
	delete this->evd;
}

/*void Creature::doAllActivities() {
 ActivityList activities = CreatureActivity::getCreatureActivities(this);
 ActivityList::iterator activity = activities.begin();
 for (; activity != activities.end(); activity++) {
 (*activity)->make();
 delete (*activity);
 }
 }*/

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

unsigned Creature::getActivitiesCount() {
	return getFenotype()->population->getCreatureActivitiesCount();
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

