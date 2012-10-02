/*
 * Population.cpp
 *
 *  Created on: 2011-10-07
 *      Author: wojtas
 */

#include "Population.h"
#include "World.h"
#include "ModelHelpers.h"
#include "Creature.h"

LoggerPtr CreaturesPopulation::logger(Logger::getLogger("population"));

bool moreValuableCreature(Creature *one, Creature *two) {
	return one->getWallet() > two->getWallet();
}

CreaturesPopulation::CreaturesPopulation(std::wstring name, int activitiesCount,
		Creature *oneCreature) {
	LOG4CXX_TRACE(logger,
			"CreaturesPopulation( std::wstring name, int activitiesCount, Creature *oneCreature)");
	this->activitiesCount = activitiesCount;
	this->name = name;
}

CreaturesPopulation::CreaturesPopulation(Field *field,
		const JSONObject &population, unsigned x, unsigned y) {
	LOG4CXX_TRACE(logger,
			"CreaturesPopulation( Field *field, const JSONObject &population, unsigned x, unsigned y)");
	//GAPopulation gaPop;
	activitiesCount = population.at(L"activitiesCount")->AsNumber();
	name = population.at(L"name")->AsString();
	LOG4CXX_DEBUG(logger, "activitiesCount = " << activitiesCount);
	if (population.count(L"creatures") > 0) {
		LOG4CXX_TRACE(logger, "Creating creatures");
		JSONArray creatures = population.at(L"creatures")->AsArray();
		JSONArray::iterator creature = creatures.begin();
		for (; creature != creatures.end(); creature++) {
			JSONObject creatureRepresentation = (*creature)->AsObject();
			Creature *creature = new Creature(this, field,
					creatureRepresentation, x, y);
			std::stringstream buf;
			buf << *creature;
			LOG4CXX_TRACE(logger,
					"adding creature " << buf.str() << "T:" << creature);
			this->creatures.push_back(creature);
		}
	}
	int randomCreaturesCount =
			population.at(L"generateRandomCreatures")->AsNumber();
	if (randomCreaturesCount > 0) {
		LOG4CXX_TRACE(logger,
				"Generating " << randomCreaturesCount << " random creatures");
		for (int i = 0; i < randomCreaturesCount; i++) {
			Creature *creature = new Creature(this, field, x, y);
			LOG4CXX_TRACE(logger,
					"adding creature " << *creature << "T:" << creature);
			creatures.push_back(creature);
		}
	}
	LOG4CXX_DEBUG(logger,
			"populationSize after creation/generation = " << creatures.size());
}

const Creature *CreaturesPopulation::selectCreature(
		SelectionStrategy selectionStrategy) const {
	// Assume creatures sorted by objective. First is the best.
	Creature *selected;
	switch (selectionStrategy) {
	case SS_RANDOM:
		selected = creatures.at(
				randBetweenAndStepped(0.0, creatures.size(), 1.0));
		break;
	case SS_WORST:
		selected = creatures.back();
		break;
	default:
		selected = creatures.front();
		break;
	}
	return selected;
}

unsigned CreaturesPopulation::getNoOfGenes() const {
	return getCreatureActivitiesCount() * 2;
}

void CreaturesPopulation::sexualCrossover(const Creature *mom,
		const Creature *dad, Creature *sister, Creature *brother) const {
	unsigned genesCount = getNoOfGenes();
	for (unsigned i = 0; i < genesCount; i++) {
		unsigned momGene = mom->gene(i);
		unsigned dadGene = dad->gene(i);
		if (flipCoin(0.5)) {
			sister->gene(i, momGene);
			brother->gene(i, dadGene);
		} else {
			sister->gene(i, dadGene);
			brother->gene(i, momGene);
		}
	}
}

void CreaturesPopulation::onePointCrossover(const Creature *mom,
		const Creature *dad, Creature *sister, Creature *brother) const {
	unsigned genesCount = getNoOfGenes();
	unsigned breakGeneIndex = randBetweenAndStepped(0, genesCount, 1);
	for (unsigned i = 0; i < genesCount; i++) {

		unsigned momGene = mom->gene(i);
		unsigned dadGene = dad->gene(i);
		if (i < breakGeneIndex) {
			sister->gene(i, momGene);
			brother->gene(i, dadGene);
		} else {
			sister->gene(i, dadGene);
			brother->gene(i, momGene);
		}
	}
}

void CreaturesPopulation::reproduce() {
	LOG4CXX_TRACE(logger, "reproduce");
	// TODO - implementacja
	if (creatures.size() <= 1)
		return;
	const Creature *mom = selectCreature(CreaturesPopulation::SS_RANDOM);
	const Creature *dad = selectCreature(CreaturesPopulation::SS_RANDOM);
	Creature *child1 = new Creature(*mom);
	Creature *child2 = new Creature(*dad);

	if (flipCoin(CROSSOVER)) {
		sexualCrossover(mom, dad, child1, child2);
	}

	child1->mutate(MUTATION);
	child2->mutate(MUTATION);

	std::sort(creatures.begin(), creatures.end(), moreValuableCreature);

	//ga->removeIndividual(creatureToRemove);
	while (creatures.size() > 30) {
		const Creature *dead = selectCreature(CreaturesPopulation::SS_WORST);
		LOG4CXX_DEBUG(logger,
				" DEAD " << " T:" << dead << " OLD:" << dead->getAge() << ", VAL:" << dead->getWallet() << " GENE: " << *dead);
		remove(dead);
	}

	LOG4CXX_DEBUG(logger,
			"New child1 " << " T:" << child1 << " OLD:" << child1->getAge() << " GENE: " << *child1);
	LOG4CXX_DEBUG(logger,
			"New child2 " << " T:" << child2 << " OLD:" << child2->getAge() << " GENE: " << *child2);

	// Blokada zmian dla innych wątków
	polulationChange.lock();

	creatures.push_back(child1);
	creatures.push_back(child2);
	std::sort(creatures.begin(), creatures.end(), moreValuableCreature);
	polulationChange.unlock();

	/*
	 if (rs == PARENT) {
	 child1 = pop->replace(child1, mom);
	 if (mom == dad) // this is a possibility, if so do worst
	 child2 = pop->replace(child2, GAPopulation::WORST);
	 else
	 child2 = pop->replace(child2, dad);
	 } else if (rs == CUSTOM) {
	 child1 = pop->replace(child1, &(rf(*child1, *pop)));
	 child2 = pop->replace(child2, &(rf(*child2, *pop)));
	 } else {
	 child1 = pop->replace(child1, rs);
	 child2 = pop->replace(child2, rs);
	 }
	 */

	//pop->evaluate(gaTrue); // allow pop-based evaluators to do their thing
}

CreaturesPopulation::~CreaturesPopulation() {
}

unsigned CreaturesPopulation::getCreatureActivitiesCount() const {
	return activitiesCount;
}

void CreaturesPopulation::remove(const Creature *creatureToRemove) {
	LOG4CXX_TRACE(logger,
			" removing creature " << *creatureToRemove << " T:" << creatureToRemove << " TC:" << ((Creature *)creatureToRemove));
	polulationChange.lock();
	Creatures::iterator toErase = std::find(creatures.begin(), creatures.end(),
			creatureToRemove);
	if (toErase != creatures.end())
		creatures.erase(toErase);
	polulationChange.unlock();
}

void CreaturesPopulation::add(Creature *creatureToAdd) {
	LOG4CXX_DEBUG(logger,
			" adding creature " << *creatureToAdd << " T:" << creatureToAdd << " TC:" << ((Creature *)creatureToAdd));
	polulationChange.lock();
	creatures.push_back(creatureToAdd);
	polulationChange.unlock();
}

CreaturesOfPopulationOnFieldVisitor::CreaturesOfPopulationOnFieldVisitor(
		CreaturesOnFieldVisitor * v) :
		visitor(v) {
}

void CreaturesOfPopulationOnFieldVisitor::visit(CreaturesPopulation *population,
		Field *field, unsigned x, unsigned y) {
	LOG4CXX_DEBUG(logger,
			"population:" << population << ", Population size: " << population->size());
	Creatures l = population->creatureList();
	Creatures::iterator i = l.begin();
	for (; i != l.end(); i++) {
		visitor->visit(*i, field, population, x, y);
	}
}

LoggerPtr CreaturesOfPopulationOnFieldVisitor::logger(
		Logger::getLogger("CreaturesVisitor"));

