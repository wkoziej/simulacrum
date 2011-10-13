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

using namespace std;
using namespace log4cxx;

class CreatureFenotype: public GAEvalData {

public:
	std::vector<float> createdProductQuants;
	std::vector<float> usedResourcesQuants;
	std::pair<unsigned, unsigned> previousFieldIdexes;
	int fieldCoordX;
	int fieldCoordY;
	unsigned yearsOld;
	float objectiveValue;
	const CreaturesPopulation *population;
	const Field *field;
	CreatureFenotype(const CreaturesPopulation *population, const Field *field);
	virtual GAEvalData* clone() const;
	virtual void copy(const GAEvalData &src);
	void clearPerformanceRatio() {
		performanceRatio = 0;
	}
	void increasePerformanceRatio(float amount) {
		performanceRatio += amount;
	}
	float getPerformanceRatio() {
		return performanceRatio;
	}
private:
	float performanceRatio;
};

class Creature: public GARealGenome {
public:
	enum Directions {
		NoDirection, DirLeft, DirRight, DirUp, DirDown
	};
	Creature(const CreaturesPopulation *population, const Field * field);
	Creature(const CreaturesPopulation *population, const Field * field,
			JSONObject &creature);
	void step ();
	void changePopulation(CreaturesPopulation *from, CreaturesPopulation *to);
	virtual ~Creature();
	float produce(float resourceQuantity, unsigned index, float &resourceUsed,
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
	int noOfNeeds();
	std::string genomeStr() const;
	CreatureFenotype *getFenotype() {
		return (CreatureFenotype *) evalData();
	}
private:

	//float getProcessingVelocity(unsigned index);
	float getProcessingRateAndProductIndex(unsigned resourceIndex,
			int &productIndex);
	static float randBetweenAndStepped(float min, float max, float step);
	static void RandomInitializer(GAGenome &g);
	static void JSONInitializer(GAGenome &g);
	static GARealAlleleSetArray allelesDefinition(
			const CreaturesPopulation *population);
	//void productMissed(float quant, unsigned productIndex);
	//GAGenome *genome;
	// float missedProductsFactor;
	// Logowanie
	static LoggerPtr logger;
	static GARealAlleleSetArray alleles;

};

#endif /* CREATURE_H_ */
