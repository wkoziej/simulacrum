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
#include "Config.h"

using namespace std;
using namespace log4cxx;

class CreatureFenotype: public GAEvalData {

public:
	std::vector<float> createdProductQuants;
	std::vector<float> usedResourcesQuants;
	std::vector<float> notSatisfiedNeedsQuants;

	std::pair<unsigned, unsigned> previousFieldIdexes;
	int fieldCoordX;
	int fieldCoordY;
	unsigned yearsOld;
	float objectiveValue;

	CreatureFenotype();
	virtual GAEvalData* clone() const;
	virtual void copy(const GAEvalData &src);
};

class Creature: public GARealGenome {
public:
	enum Directions {
		NoDirection, DirLeft, DirRight, DirUp, DirDown
	};
	Creature();
	virtual ~Creature();
	float produce(float resourceQuantity, unsigned index);
	void feed(float productAmount, unsigned productIndex);
	float getNeedOfProduct(unsigned productIndex);
	float getNeedOfResource(unsigned productIndex);
	//float getNeed
	Directions nextDirection(unsigned step);
	static LoggerPtr getLogger() {
		return logger;
	}
	;
	static int noOfTalents();
	static int noOfNeeds();
	CreatureFenotype *getFenotype() {
		return (CreatureFenotype *) evalData();
	}
private:

	//float getProcessingVelocity(unsigned index);
	float getProcessingRateAndProductIndex(unsigned resourceIndex, int &productIndex);
	static float randBetweenAndStepped(float min, float max, float step);
	static void RandomInitializer(GAGenome &g);
	static GARealAlleleSetArray allelesDefinition();
	//void productMissed(float quant, unsigned productIndex);
	//GAGenome *genome;
	// float missedProductsFactor;
	// Logowanie
	static LoggerPtr logger;
	static GARealAlleleSetArray alleles;
};

#endif /* CREATURE_H_ */
