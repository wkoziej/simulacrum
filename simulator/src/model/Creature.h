/*
 * Creature.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#ifndef CREATURE_H_
#define CREATURE_H_

#include <vector>
#include <ga/GAGenome.h>
#include <ga/GARealGenome.h>
#include <log4cxx/logger.h>
#include "JSON/JSON.h"
#include "Config.h"
#include "Population.h"

#include <QtCore/qthread.h>

using namespace std;
using namespace log4cxx;

float Objective(GAGenome &g);

class CreatureFenotype;
class Field;

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

	/*	float produce(float resourceQuantity, unsigned index, float &resourceUsed,
	 int &productIndex);
	 void feed(float productAmount, unsigned productIndex, float &eaten);
	 float getNeedOfProductRatio(unsigned productIndex);
	 float getNeedOfResource(unsigned productIndex);
	 void prepare4Meal() {
	 getFenotype()->clearPerformanceRatio();
	 }

	 float getPerformanceRatio() {
	 float ratio = getFenotype()->getPerformanceRatio();
	 return ratio == 0 ? 0.001 : ratio;
	 }

	 Directions nextDirection(unsigned step);
	 static LoggerPtr getLogger() {
	 return logger;
	 }

	 int noOfTalents();
	 int noOfNeeds();*/

	std::string genomeStr() const;
	CreatureFenotype *getFenotype() const {
		return (CreatureFenotype *) evalData();
	}
	Field *getField();
	unsigned getX() const;
	unsigned getY() const;

	bool hasEnergy() const;
	void rest();
	bool produce(unsigned articleId, const std::vector<float> &ingredients);
	bool collect(unsigned articleId);
	bool leave(unsigned articleId);
	bool move(unsigned x, unsigned y);

	// Logowanie
	static LoggerPtr logger;
private:
	CreatureActivityList getCreatureActivities();
	//float getProcessingVelocity(unsigned index);
	/*	float getProcessingRateAndProductIndex(unsigned resourceIndex,
	 int &productIndex);*/
	static float randBetweenAndStepped(float min, float max, float step);
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
	ActivitiesStrategy getActiviviesStrategy() const;
};

class CreatureActivityThread: public QThread {
public:
	CreatureActivityThread(Creature *creature);
	virtual ~CreatureActivityThread();
	void run();
private:
	Creature *creature;
};

#endif /* CREATURE_H_ */
