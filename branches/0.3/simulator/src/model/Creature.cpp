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
#include "QtCore/qmutex.h"
LoggerPtr Creature::logger(Logger::getLogger("creature"));

// Liczba róznych strategii wyboru aktywności
// przykłady :
// 1. Czynności o rosnącej liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 2. Czynności o malejącej  liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 3. Na zmianę zaczynając od czynności od najmniejszej liczby argumentów
#define ENERGY_INDEX 0

CreatureSignalEmmiter::CreatureSignalEmmiter() :
		QObject(NULL) {

}

CreatureSignalEmmiter::CreatureSignalEmmiter(const CreatureSignalEmmiter&) :
		QObject(NULL) {

}

void CreatureSignalEmmiter::bornSignal(QString id, unsigned atX, unsigned atY) {
	emit creatureBorn(id, atX, atY);
}
void CreatureSignalEmmiter::dieSignal(QString id, unsigned atX, unsigned atY) {
	emit creatureDie(id, atX, atY);
}

void CreatureSignalEmmiter::movedSignal(QString creatureId, unsigned xFrom,
		unsigned yFrom, unsigned xTo, unsigned yTo) {
	emit creatureMoved(creatureId, xFrom, yFrom, xTo, yTo);
}

void CreatureSignalEmmiter::ateSignal(QString creatureId, unsigned articleId) {
	emit creatureAte(creatureId, articleId);
}

class CreatureFenotype /*: public GAEvalData*/{

public:
	unsigned fieldCoordX;
	unsigned fieldCoordY;
	unsigned yearsOld;
	QString id;
	std::wstring speciesName;
	float wallet;
	UnsignedVector articleStocks;
	IntVector articleQuantsChange;
	CreatureSignalEmmiter emmiter;
	CreatureFenotype(std::wstring species, unsigned x, unsigned y);

	CreatureFenotype* clone() const;
	void copy(const CreatureFenotype &src);

private:
	static LoggerPtr logger;
	static unsigned createuresId;
	void assignId();
};

unsigned CreatureFenotype::createuresId = 0;

float Objective(GAGenome &g) {
	Creature *c = (Creature *) &g;
	CreatureFenotype * fenotype = c->getFenotype();
	float value = 0.0;
	//World *world = World::getWorld();
	//Field *field = fenotype->field;
	// Możliwe funkcje celu
	// 1. Stosunek wartości wytworzonych produktów do wartości zużytych surowców jest największy
	// 2. Wartość wytworzonych produktów jest największa

	// wersja 1
	// Wartość wytworzonych produktów
	/*
	 float articlesValueSum = 0.0;
	 for (unsigned i = 0; i < World::NO_OF_ARTICLES; i++) {
	 articlesValueSum += field->getMarket()->articleBuyPrice(i)
	 * fenotype->articleStocks.at(i);
	 }
	 */

	value = fenotype->wallet;

	/*	if (!c->getId().empty()) {
	 LOG4CXX_DEBUG(Creature::logger, "creature " <<c->getId() << " objective = " << value);
	 }*/
	return value;
}

LoggerPtr CreatureFenotype::logger(Logger::getLogger("creatureFenotype"));

CreatureFenotype::CreatureFenotype(std::wstring species, unsigned x,
		unsigned y) {
	LOG4CXX_TRACE(logger, "CreatureFenotype::CreatureFenotype");
	fieldCoordX = x;
	fieldCoordY = y;
	articleQuantsChange.assign(World::NO_OF_ARTICLES, 0);
	articleStocks.assign(World::NO_OF_ARTICLES, 0);
	yearsOld = 0;
	speciesName = species;
	wallet = 0.0;
	assignId();
}
;

CreatureFenotype* CreatureFenotype::clone() const {
	LOG4CXX_TRACE(logger, "CreatureFenotype::clone");
	CreatureFenotype *cf = new CreatureFenotype(this->speciesName,
			this->fieldCoordX, this->fieldCoordY);
	cf->copy(*this);
	return cf;
}

void CreatureFenotype::copy(const CreatureFenotype&src) {
	LOG4CXX_TRACE(logger, ">CreatureFenotype::copy");
	CreatureFenotype source = (CreatureFenotype &) src;

	articleQuantsChange.assign(World::NO_OF_ARTICLES, 0);
	articleStocks.assign(World::NO_OF_ARTICLES, 0);

	yearsOld = 0;
	speciesName = source.speciesName;
	wallet = 0;
	fieldCoordX = source.fieldCoordX;
	fieldCoordY = source.fieldCoordY;
	assignId();
	LOG4CXX_TRACE(logger, "<CreatureFenotype::copy");
}

void CreatureFenotype::assignId() {
	std::stringstream idBuf;
	idBuf << CreatureFenotype::createuresId++;
	id = idBuf.str().c_str();
	LOG4CXX_DEBUG(logger,
			"craeture ID = " << id.toStdString() << ", CreatureFenotype::createuresId " << CreatureFenotype::createuresId);

}

//GARealAlleleSetArray Creature::alleles;

std::string Creature::genomeStr() const {
	std::stringstream buf;
	// kodowanie czynnosci
	Genome::const_iterator g = genome.begin();
	for (; g != genome.end();) {
		int activity = *(g++);
		int argument = *(g++);
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

void Creature::RandomInitializer(Genome &g, unsigned genomeLenght) {
	unsigned i = 0;
	unsigned c = genomeLenght;
	for (; i < c; i++) {
		// Indeks czynnosci
		g.push_back(randBetweenAndStepped(0, Activity::COUNT - 1, 1));
		// Indeks produktu
		g.push_back(randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1));
	}
}

void Creature::JSONInitializer(Genome &g, JSONObject *creature) {
	//Creature *h = (Creature *) &g;
	//JSONObject *creature = (JSONObject *) ((GAGenome *) h)->userData();
	unsigned i;
	// kodowanie czynnosci 1 argumentowych
	JSONArray activities = creature->at(L"activities")->AsArray();
	unsigned activitiesSize = activities.size();
	// TODO - nie sprawdzamy, czy wczytywany osobnik ma odpowiednią liczbę genów, a warto byłoby aby w populacji wszystkie miały równą
	for (i = 0; i < activitiesSize; i++) {
		JSONObject activity = activities.at(i)->AsObject();
		// czynnosc
		std::string activityName = wstring2string(activity.begin()->first);
		unsigned activityIndex = Activity::getIndex(activityName);
		g.push_back(activityIndex);
		unsigned articleIndex = 0;
		if (activity.begin()->second->IsNull()) {
			articleIndex = rand() % World::ARTICLES.size();
		} else {
			// argument
			articleIndex = World::getArticleIndex(
					activity.begin()->second->AsString());
		}
		g.push_back(articleIndex);
	}
}

Creature::Creature(CreaturesPopulation *population, Field * field,
		JSONObject &creature, unsigned x, unsigned y) {
	LOG4CXX_TRACE(logger,
			"Creature::Creature(CreaturesPopulation *population, Field * field, JSONObject &creature, unsigned x, unsigned y)");
	// Wypełnij tymczasową strukturę z definicją osobnika
	fenotype = new CreatureFenotype(population->getName(), x, y);
	Creature::JSONInitializer(genome, &creature);
	//initialize();
	// Można już wyczyścić dane
	//GAGenome::userData(NULL);
	//initializer(DoNothingInitializer);
	World::connectCreatureSignals(this);
	getFenotype()->emmiter.bornSignal(getId(), x, y);
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

Creature::Creature(CreaturesPopulation *population, Field * field, unsigned x,
		unsigned y) {
	LOG4CXX_TRACE(logger,
			"Creature::Creature(CreaturesPopulation *population, Field * field, unsigned x, unsigned y)");
	LOG4CXX_DEBUG(logger, "genome before init : " << *this);
	// Wypełnij tymczasową strukturę z definicją osobnika
	fenotype = new CreatureFenotype(population->getName(), x, y);
	Creature::RandomInitializer(genome,
			population->getCreatureActivitiesCount());
	World::connectCreatureSignals(this);
	LOG4CXX_DEBUG(logger, "genome after init: " << *this);
	getFenotype()->emmiter.bornSignal(getId(), x, y);
}

Creature::Creature(const Creature &creature) {
	LOG4CXX_TRACE(logger, "Creature::Creature(const Creature &creature)");
	genome = creature.genome;
	fenotype = new CreatureFenotype(creature.getFenotype()->speciesName,
			creature.getFenotype()->fieldCoordX,
			creature.getFenotype()->fieldCoordY);
	World::connectCreatureSignals(this);
	getFenotype()->emmiter.bornSignal(getId(), getX(), getY());
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

Creature::~Creature() {
	LOG4CXX_DEBUG(logger, "Creature::~Creature " << getId().toStdString());
	delete fenotype;
	fenotype = NULL;
}

Field *Creature::getField() {
	return World::getPtr()->fields.at(getX()).at(getY());
}

bool Creature::hasEnergy() const {
	return getFenotype()->articleStocks.at(ENERGY_INDEX) > 0;
}

void Creature::rest() {
	LOG4CXX_DEBUG(logger, "craeture " << getId().toStdString() << " resting ");
	modifyStocks(ENERGY_INDEX, 1);
}

bool Creature::produce(unsigned articleId, std::vector<unsigned> ingredients) {
	LOG4CXX_TRACE(logger,
			"craeture " << getId().toStdString() << " trying to produce " << World::ARTICLES.at(articleId));
	bool produced = true;
	assert(getFenotype()->articleStocks.size() == ingredients.size());
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
		unsigned index = 0;
		LOG4CXX_DEBUG(logger,
				"craeture " << getId().toStdString() << " producing " << World::ARTICLES.at(articleId));
		for (; i != ingredients.end(); s++, i++, index++) {
			LOG4CXX_DEBUG(logger,
					"craeture " << getId().toStdString() << " using " << *i << " of " << World::ARTICLES.at(index) << " to produce " << World::ARTICLES.at(articleId));
			modifyStocks(index, -(*i));
		}
		modifyStocks(articleId, 1);
	} else {
		wrongDecisionSanction();
	}
	return produced;
}

bool Creature::collect(unsigned articleId) {
	LOG4CXX_TRACE(logger,
			"craeture " << getId().toStdString() << " trying to collect " << World::ARTICLES.at(articleId));
	bool canCollect = getField()->tryTakeArticle(articleId);
	if (canCollect) {
		LOG4CXX_DEBUG(logger,
				"craeture " << getId().toStdString() << " collecting " << World::ARTICLES.at(articleId));
		modifyStocks(articleId, 1);
	} else {
		wrongDecisionSanction();
	}
	return canCollect;
}

bool Creature::leave(unsigned articleId) {
	LOG4CXX_TRACE(logger,
			"craeture " << getId().toStdString() << " trying to leave " << World::ARTICLES.at(articleId));
	CreatureFenotype *fenotype = getFenotype();
	bool canLeave = fenotype->articleStocks.at(articleId) >= 1.0;
	if (canLeave) {
		LOG4CXX_DEBUG(logger,
				"craeture " << getId().toStdString() << " leaving " << World::ARTICLES.at(articleId));
		modifyStocks(articleId, -1);
		Field *field = getField();
		field->putArticle(articleId);
	} else {
		wrongDecisionSanction();
	}
	return canLeave;
}

bool Creature::sell(unsigned articleId) {
	LOG4CXX_TRACE(logger,
			"craeture " << getId().toStdString() << " trying to sell " << World::ARTICLES.at(articleId));
	CreatureFenotype *fenotype = getFenotype();

	bool canSell = fenotype->articleStocks.at(articleId) >= 1.0;
	if (canSell) {
		LOG4CXX_DEBUG(logger,
				"craeture " << getId().toStdString() << " selling " << World::ARTICLES.at(articleId));
		Field *field = getField();
		float moneyEarned = field->getMarket()->buyArticleFromClient(
				getId().toStdString(), articleId);
		fenotype->wallet += moneyEarned;
		modifyStocks(articleId, -1);
	} else {
		wrongDecisionSanction();
	}
	return canSell;
}

bool Creature::buy(unsigned articleId) {
	LOG4CXX_TRACE(logger,
			"craeture " << getId().toStdString() << " trying to buy " << World::ARTICLES.at(articleId));

	CreatureFenotype *fenotype = getFenotype();
	Field *field = getField();
	bool bought = field->getMarket()->sellArticleToClient(getId().toStdString(),
			articleId, fenotype->wallet);
	if (bought) {
		LOG4CXX_DEBUG(logger,
				"craeture " << getId().toStdString() << " buying " << World::ARTICLES.at(articleId));
		modifyStocks(articleId, 1);
	} else {
		wrongDecisionSanction();
	}
	return bought;
}

bool Creature::check(unsigned articleId) {
	LOG4CXX_DEBUG(logger,
			"craeture " << getId().toStdString() << " checking " << World::ARTICLES.at(articleId));
	Field *field = getField();
	field->getMarket()->articleSellPrice(getId().toStdString(), articleId);
	return true;
}

bool Creature::eat(unsigned articleId) {
	LOG4CXX_TRACE(logger,
			"creature " << getId().toStdString() << " traying to eat " << World::ARTICLES.at(articleId));
	CreatureFenotype *fenotype = getFenotype();
	bool eaten = fenotype->articleStocks.at(articleId) > 0;
	if (fenotype->articleStocks.at(articleId) > 0) {
		modifyStocks(articleId, -1);
		bool eatFood = World::articles.at(articleId)->isFood();
		if (eatFood) {
			LOG4CXX_DEBUG(logger,
					"creature " << getId().toStdString() << " eating " << World::ARTICLES.at(articleId));
			modifyStocks(ENERGY_INDEX, 1);
			getFenotype()->emmiter.ateSignal(getId(), articleId);
		} else {
			wrongDecisionSanction();
			if (hasEnergy()) {
				modifyStocks(ENERGY_INDEX, -1);
			} else {
				// One more sanction
				wrongDecisionSanction();
			}
		}
	}
	return eaten;
}

unsigned Creature::getX() const {
	return getFenotype()->fieldCoordX;
}

unsigned Creature::getY() const {
	return getFenotype()->fieldCoordY;
}

QString Creature::getId() const {
	QString str("%1");
	return str.arg(getFenotype()->id); //.arg((long) this);
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

unsigned Creature::getActivitiesCount() const {
	// 1 - typ czynnosci
	// 2 - argument
	return genome.size() / 2;
}
/*
 std::wstring Creature::getPopulationName() const {
 return getFenotype()->speciesName;
 }
 */

bool Creature::move(unsigned x, unsigned y) {
	LOG4CXX_TRACE(logger,
			"creature " << getId().toStdString() << " T:" << this << " want to move from " << getFenotype()->fieldCoordX << "x" << getFenotype()->fieldCoordY << " to " << x << "x" << y);
	bool anotherField = (x != getFenotype()->fieldCoordX
			|| y != getFenotype()->fieldCoordY);
	bool moved = anotherField && hasEnergy();
	if (moved) {
		World *w = World::getPtr();
		Field *creatureField = getField();
		CreaturesPopulation *population = creatureField->getPopulation(
				getFenotype()->speciesName);
		LOG4CXX_DEBUG(logger,
				"creature " << getId().toStdString() << " T:" << this << " moving from " << getFenotype()->fieldCoordX << "x" << getFenotype()->fieldCoordY << " to " << x << "x" << y);
		population->remove(this);
		w->insertCreatureToXYPopulation(population->getName(),
				getActivitiesCount(), x, y, this);

		unsigned xFrom = x;
		unsigned yFrom = y;

		// GADemeGA
		getFenotype()->fieldCoordX = x;
		getFenotype()->fieldCoordY = y;
		modifyStocks(ENERGY_INDEX, -1);

		LOG4CXX_DEBUG(logger,
				"creature " << getId().toStdString() << " T:" << this << " moved to " << x << "x" << y);

		getFenotype()->emmiter.movedSignal(getId(), xFrom, yFrom, x, y);

		LOG4CXX_DEBUG(logger,
				"creature " << getId().toStdString() << " emited signal of move");

	} else {
		wrongDecisionSanction();
		LOG4CXX_DEBUG(logger,
				"creature " << getId().toStdString() << " cannot move to " << x << "x" << y);

	}

	return moved;
}

int Creature::increaseAge() {
	getFenotype()->wallet = 0.0;
	return ++getFenotype()->yearsOld;
}

void Creature::mutate(float probability) {
	Genome::iterator g = genome.begin();
	for (; g != genome.end();) {
		// Czynność
		if (flipCoin(probability)) {
			*g = randBetweenAndStepped(0, Activity::COUNT - 1, 1);
		}
		g++;
		if (flipCoin(probability)) {
			// Indeks produktu
			*g = randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1);
		}
		g++;
	}
}

void Creature::prepareToDie() {
	LOG4CXX_DEBUG(logger,
			"creature " << getId().toStdString() << " preparing to die ");
	UnsignedVector::iterator s = getFenotype()->articleStocks.begin();
	unsigned articleId = 0;
	for (; s != getFenotype()->articleStocks.end(); s++, articleId++) {
		if (*s > 0) {
			LOG4CXX_DEBUG(logger,
					"creature " << getId().toStdString() << " leaving " << *s<< " of " << World::ARTICLES.at(articleId));
			Field *field = getField();
			field->putArticle(articleId, *s);
			modifyStocks(articleId, -(*s));
		}
	}
	getFenotype()->emmiter.dieSignal(getId(), getX(), getY());
}

void Creature::modifyStocks(unsigned articleId, int delta) {
	LOG4CXX_DEBUG(logger,
			"creature's " << getId().toStdString() << " stock of " << World::ARTICLES.at(articleId) << " modified by " << delta);
	int q = getFenotype()->articleStocks.at(articleId);
	assert(q + delta >= 0);
	getFenotype()->articleStocks.at(articleId) += delta;
	getFenotype()->articleQuantsChange.at(articleId) += delta;
}

void Creature::wrongDecisionSanction(float weight) {
	float w = getFenotype()->wallet;
	w -= weight / (float) getActivitiesCount();
	getFenotype()->wallet = w < 0.0 ? 0 : w;

}

inline std::ostream & operator<<(std::ostream& os, const Creature& creature) {
	os << creature.genomeStr() << " ";
	return os;
}

