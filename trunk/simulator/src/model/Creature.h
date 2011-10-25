/*
 * Creature.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */
#ifndef CREATURE_H_
#define CREATURE_H_

#include <list>
//#include <ga/GAGenome.h>
#include <ga/GARealGenome.h>
#include <log4cxx/logger.h>
#include <QtCore/qobject.h>
#include "Config.h"
#include "Types.h"
//#include "Population.h"
#include "JSON/JSON.h"

//using namespace std;
using namespace log4cxx;
//

float Objective(GAGenome &g);

class CreatureFenotype;
class Field;
class CreaturesPopulation;

class CreatureActivity;

class Creature: public QObject, public GARealGenome {
Q_OBJECT
signals:
	void creatureBorn(QString, unsigned atX, unsigned atY);
	void creatureMoved(QString creatureId, unsigned xFrom, unsigned yFrom,
			unsigned xTo, unsigned yTo);
	void creatureAte(QString creatureId, unsigned articleId);

public:
	// Logowanie
	static LoggerPtr logger;
	Creature(const Creature &creature);
	Creature(CreaturesPopulation *population, Field * field,
			JSONObject &creature, unsigned x, unsigned y);
	Creature(CreaturesPopulation *population, Field * field, unsigned x,
			unsigned y);
	Creature & operator=(const Creature & arg) {
		LOG4CXX_TRACE(logger, "operator=");
		copy(arg);
		return *this;
	}
	//virtual Creature* clone(GAGenome::CloneMethod flag = CONTENTS);
	//virtual void copy(const Creature&);


	virtual GAGenome* clone(GAGenome::CloneMethod flag = CONTENTS);
	virtual void copy(const GAGenome&);

	virtual ~Creature();

	//void doAllActivities();

	std::string genomeStr() const;
	CreatureFenotype *getFenotype() const {
		return (CreatureFenotype *) evalData();
	}
	Field *getField();
	unsigned getX() const;
	unsigned getY() const;
	std::string getId() const;
	unsigned getAge() const;
	float getWallet() const;
	unsigned getArticleStock(unsigned articleId) const;
	int getArticleQuantChange(unsigned articleId) const;
	unsigned getActivitiesCount();
	bool hasEnergy() const;
	void rest();
	bool produce(unsigned articleId, std::vector<unsigned> ingredients);
	bool collect(unsigned articleId);
	bool leave(unsigned articleId);
	bool sell(unsigned articleId);
	bool buy(unsigned articleId);
	bool check(unsigned articleId);
	bool eat(unsigned articleId);
	bool move(unsigned x, unsigned y);

	int increaseAge();
	// Leave all stocks on field
	void prepareToDie();

private:
	static unsigned createuresId;
	static void RandomInitializer(GAGenome &g);
	static void DoNothingInitializer(GAGenome &g);
	static void JSONInitializer(GAGenome &g);
	static GARealAlleleSetArray allelesDefinition(
			const CreaturesPopulation *population);

	static GARealAlleleSetArray alleles;
	void changePopulation(CreaturesPopulation *from, CreaturesPopulation *to);
	CreatureActivity *createActivity(unsigned activityGenIndex,
			unsigned parametersCount);
	void modifyStocks(unsigned articleId, int delta);
	void wrongDecisionSanction(float weight = 1.0);
	void assignId();

};

#endif /* CREATURE_H_ */
