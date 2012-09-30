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
//#include <ga/GARealGenome.h>
#include <log4cxx/logger.h>
#include <QtCore/qobject.h>
#include "Config.h"
#include "Types.h"

//#include "Population.h"
#include "JSON/JSON.h"

typedef std::vector<unsigned> Genome;

//using namespace std;
using namespace log4cxx;
//

//float Objective(GAGenome &g);

class CreatureFenotype;
class Field;
class CreaturesPopulation;

class CreatureActivity;

class CreatureSignalEmmiter: public QObject {
Q_OBJECT
public:
	CreatureSignalEmmiter();
	CreatureSignalEmmiter(const CreatureSignalEmmiter&);
	void bornSignal(QString, unsigned atX, unsigned atY);
	void dieSignal(QString, unsigned atX, unsigned atY);
	void movedSignal(QString creatureId, unsigned xFrom, unsigned yFrom,
			unsigned xTo, unsigned yTo);
	void ateSignal(QString creatureId, unsigned articleId);

signals:
	void creatureBorn(QString, unsigned atX, unsigned atY);
	void creatureDie(QString, unsigned atX, unsigned atY);
	void creatureMoved(QString creatureId, unsigned xFrom, unsigned yFrom,
			unsigned xTo, unsigned yTo);
	void creatureAte(QString creatureId, unsigned articleId);

};

class Creature /*public QObject, public GARealGenome */{
	/*
	 Q_OBJECT
	 signals:
	 void creatureBorn(QString, unsigned atX, unsigned atY);
	 void creatureDie(QString, unsigned atX, unsigned atY);
	 void creatureMoved(QString creatureId, unsigned xFrom, unsigned yFrom,
	 unsigned xTo, unsigned yTo);
	 void creatureAte(QString creatureId, unsigned articleId);
	 */
public:
	// Logowanie
	static LoggerPtr logger;
	Creature(const Creature &creature);
	Creature(CreaturesPopulation *population, Field * field,
			JSONObject &creature, unsigned x, unsigned y);
	Creature(CreaturesPopulation *population, Field * field, unsigned x,
			unsigned y);
	/*
	 Creature & operator=(const Creature & arg) {
	 LOG4CXX_TRACE(logger, "operator=");
	 copy(arg);
	 return *this;
	 }
	 */
	//virtual Creature* clone(GAGenome::CloneMethod flag = CONTENTS);
	//virtual void copy(const Creature&);
	//virtual GAGenome* clone(GAGenome::CloneMethod flag = CONTENTS);
	//virtual void copy(const GAGenome&);
	virtual ~Creature();

	std::string genomeStr() const;
	CreatureFenotype *getFenotype() const {
		return fenotype;
	}
	Field *getField();
	unsigned getX() const;
	unsigned getY() const;
	QString getId() const;
	unsigned getAge() const;
	float getWallet() const;
	unsigned getArticleStock(unsigned articleId) const;
	int getArticleQuantChange(unsigned articleId) const;
	unsigned getActivitiesCount() const;
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

	void mutate (float probability);

	int increaseAge();
	// Leave all stocks on field
	void prepareToDie();
	unsigned gene(unsigned index) const {
		return genome.at(index);
	}
	void gene(unsigned index, unsigned genValue) {
		genome [index] = genValue;
	}

private:
	Genome genome;
	CreatureFenotype *fenotype;
	static void RandomInitializer(Genome &g, unsigned genomeLenght);
	//static void DoNothingInitializer(Genome &g);
	static void JSONInitializer(Genome &g, JSONObject *creature);

	void changePopulation(CreaturesPopulation *from, CreaturesPopulation *to);
	CreatureActivity *createActivity(unsigned activityGenIndex,
			unsigned parametersCount);
	void modifyStocks(unsigned articleId, int delta);
	void wrongDecisionSanction(float weight = 1.0);
};

std::ostream & operator<<(std::ostream& os, const Creature& creature);

#endif /* CREATURE_H_ */
