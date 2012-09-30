/*
 * GeneticAlgorithm.h
 *
 *  Created on: 29-09-2012
 *      Author: wojtas
 */

#ifndef GENETICALGORITHM_H_
#define GENETICALGORITHM_H_
#include "ga/GAIncGA.h"
#include <log4cxx/logger.h>
using namespace log4cxx;
class GeneticAlgorithm: public GAIncrementalGA {
public:
	GADefineIdentity("GeneticAlgorithm", 200)
	;
	GeneticAlgorithm(const GAPopulation&);
	GeneticAlgorithm(GAGenome *creature);
	virtual ~GeneticAlgorithm();
	void removeIndividual(GAGenome *creatureToRemove);
	void addIndividual(GAGenome *creatureToAdd);
	virtual void step();
private:
	// Logowanie
	static LoggerPtr logger;
};

#endif /* GENETICALGORITHM_H_ */
