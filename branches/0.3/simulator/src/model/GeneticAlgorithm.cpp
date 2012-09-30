/*
 * GeneticAlgorithm.cpp
 *
 *  Created on: 29-09-2012
 *      Author: wojtas
 */

#include "GeneticAlgorithm.h"

LoggerPtr GeneticAlgorithm::logger(Logger::getLogger("geneticAlgorithm"));

GeneticAlgorithm::GeneticAlgorithm(const GAPopulation& population) :
		GAIncrementalGA(population) {
}

GeneticAlgorithm::GeneticAlgorithm(GAGenome *creature) :
		GAIncrementalGA(*creature) {
	LOG4CXX_TRACE(logger, "CGeneticAlgorithm(GAGenome *creature)");
	pop->size(0);
	GAGenome *added = pop->add(creature);
	LOG4CXX_DEBUG(logger,
			"creature << " << *creature << " / " << &(pop->individual(0)) << " : adding: " << creature << ", added: " << added);
	assert(added == creature);
}

GeneticAlgorithm::~GeneticAlgorithm() {
}

void GeneticAlgorithm::removeIndividual(GAGenome *creatureToRemove) {
	LOG4CXX_DEBUG(logger,
			" removing creature " << *creatureToRemove << " T:" << creatureToRemove);
	GAGenome *removed = pop->remove(creatureToRemove);
	LOG4CXX_DEBUG(logger,
			"creatureToRemove: " << creatureToRemove << ", removed : " << removed);
	assert(removed == creatureToRemove);
}

void GeneticAlgorithm::addIndividual(GAGenome *creatureToAdd) {
	pop->add(creatureToAdd);
}

void GeneticAlgorithm::step() {
// Zwierzaki mogły rozłazić się po świecie i nowe się pojawić w naszej populacji, zatem aktualizujemy statystyki przed reprodukcją
	pop->evaluate(gaTrue);
	stats.update(*pop);
	if (pop->size() > 1)
		GAIncrementalGA::step();
}
