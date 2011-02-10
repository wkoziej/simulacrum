/*
 * Creature.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#ifndef CREATURE_H_
#define CREATURE_H_

#include <ga/GAGenome.h>
#include <ga/GARealGenome.h>
#include <log4cxx/logger.h>
using namespace log4cxx;

class Creature :public GARealGenome {
public:
	enum Directions {
		NoDirection, DirLeft, DirRight, DirUp, DirDown
	};
	Creature();
	virtual ~Creature();
	float produce(float resourceQuantity, float &resourceUsed, unsigned index);
	void feed(float productAmount, float &productEaten, bool &amountSuffice,
			unsigned productIndex);
	//GAGenome *getGenome();
	//bool dying();
	float getVelocity ();
	Directions nextDirection(unsigned step);
private:
	float getNeedAmount(unsigned productIndex);
	float getProcessingVelocity(unsigned index);
	float getProcessingRate(unsigned index);
	static float randBetweenAndStepped (float min, float max, float step);
	static void RandomInitializer (GAGenome &g);
	static GARealAlleleSetArray allelesDefinition();
	//GAGenome *genome;
	// float missedProductsFactor;
	// Logowanie
	static LoggerPtr logger;
	static GARealAlleleSetArray alleles;
};

#endif /* CREATURE_H_ */
