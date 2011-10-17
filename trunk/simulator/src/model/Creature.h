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
//#include <QtCore/qthread.h>
#include "Config.h"
//#include "Population.h"
#include "JSON/JSON.h"


//using namespace std;
using namespace log4cxx;
//

float Objective(GAGenome &g);

class CreatureFenotype;
class Field;
class CreaturesPopulation;


enum ActivitiesStrategy {
	AscengingArgumentCountOrder
};

enum ZeroArgActivities {
	GoUp, GoDown, GoLeft, GoRight, RestActivity, ZeroArgActivitiesSIZE
};

enum OneArgActivities {
	ProduceArticle,
	CollectArticle,
	LeaveArticle,
	SellArticle,
	BuyArticle,
	CheckArticle,
	OneArgActivitiesSIZE
};

enum TwoArgActivities {
	ExchangeArticles, TwoArgActivitiesSIZE
};

class CreatureActivity;
typedef std::list<CreatureActivity *> CreatureActivityList;

class Creature: public GARealGenome {
public:
	// Logowanie
	static LoggerPtr logger;
	enum Directions {
		NoDirection, DirLeft, DirRight, DirUp, DirDown
	};
	Creature(const Creature &creature);
	//Creature(const CreaturesPopulation *population, const Field * field);
	Creature(CreaturesPopulation *population, Field * field,
			JSONObject &creature);
	Creature & operator=(const Creature & arg) {
		copy(arg);
		return *this;
	}
	virtual Creature* clone(GAGenome::CloneMethod flag = CONTENTS);
	virtual void copy(const Creature&);
	virtual ~Creature();

	void doAllActivities();

	std::string genomeStr() const;
	CreatureFenotype *getFenotype() const {
		return (CreatureFenotype *) evalData();
	}
	Field *getField();
	unsigned getX() const;
	unsigned getY() const;
	std::string getId () const;
	unsigned getAge () const;
	unsigned getArticleStock (unsigned articleId) const;
	int getArticleQuantChange (unsigned articleId) const;

	bool hasEnergy() const;
	void rest();
	bool produce(unsigned articleId, std::vector<unsigned> ingredients);
	bool collect(unsigned articleId);
	bool leave(unsigned articleId);
	bool sell(unsigned articleId);
	bool buy(unsigned articleId);
	bool check(unsigned articleId);
	bool move(unsigned x, unsigned y);

private:
	static unsigned createuresId;
	CreatureActivityList getCreatureActivities();
	//float getProcessingVelocity(unsigned index);
	/*	float getProcessingRateAndProductIndex(unsigned resourceIndex,
	 int &productIndex);*/
	//static float randBetweenAndStepped(float min, float max, float step);
	//static void RandomInitializer(GAGenome &g);
	static void DoNothingInitializer(GAGenome &g);
	static void JSONInitializer(GAGenome &g);
	static GARealAlleleSetArray allelesDefinition(
			const CreaturesPopulation *population);
	//void productMissed(float quant, unsigned productIndex);
	//GAGenome *genome;
	// float missedProductsFactor;

	static GARealAlleleSetArray alleles;
	//	void step ();
	void changePopulation(CreaturesPopulation *from, CreaturesPopulation *to);
	CreatureActivity *createActivity(unsigned activityGenIndex,
			unsigned parametersCount);
	//ActivitiesStrategy getActiviviesStrategy() const;
};


#endif /* CREATURE_H_ */
