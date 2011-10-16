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
#include "Field.h"
#include <QtCore/qthread.h>

using namespace std;
using namespace log4cxx;

float Objective(GAGenome &g);

class CreatureFenotype: public GAEvalData {

public:
	FloatVector gainedArticlesQuants;
	FloatVector lostArticlesQuants;
	std::pair<unsigned, unsigned> previousFieldIdexes;
	int fieldCoordX;
	int fieldCoordY;
	unsigned yearsOld;
	const CreaturesPopulation *population;
	const Field *field;

	float wallet;
	FloatVector articleStocks;

	CreatureFenotype(const CreaturesPopulation *population, const Field *field);
	virtual GAEvalData* clone() const;
	virtual void copy(const GAEvalData &src);
	/*void clearPerformanceRatio() {
	 performanceRatio = 0;
	 }
	 void increasePerformanceRatio(float amount) {
	 performanceRatio += amount;
	 }
	 float getPerformanceRatio() {
	 return performanceRatio;
	 }*/
private:
	float performanceRatio;
	static LoggerPtr logger;
};

class Creature: public GARealGenome {
public:
	enum Directions {
		NoDirection, DirLeft, DirRight, DirUp, DirDown
	};
	Creature(const Creature &creature);
	Creature(const CreaturesPopulation *population, const Field * field);
	Creature(const CreaturesPopulation *population, const Field * field,
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
	CreatureFenotype *getFenotype() {
		return (CreatureFenotype *) evalData();
	}
private:

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
	// Logowanie
	static LoggerPtr logger;
	static GARealAlleleSetArray alleles;
	//	void step ();
	void changePopulation(CreaturesPopulation *from, CreaturesPopulation *to);

};

class CreatureActivity {
private:
	CreatureFenotype *fenotype;
	Field *field;
	FloatVector arguments;
public:
	CreatureActivity(Creature *creature, FloatVector &arguments) {
		this->fenotype = creature->getFenotype();
		this->field = this->fenotype->field;
		copy(arguments.begin(), arguments.end(), this->arguments);
	}
	virtual void make() = 0;
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
