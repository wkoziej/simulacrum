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

CreaturesPopulation::CreaturesPopulation(std::wstring name, int activitiesCount,
		Creature *oneCreature) {
	LOG4CXX_TRACE(logger,
			"CreaturesPopulation( std::wstring name, int activitiesCount, Creature *oneCreature)");
	this->activitiesCount = activitiesCount;
	this->name = name;
	///ga = new GeneticAlgorithm(oneCreature);
	//ga->initialize(time(0));
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

const Creature *CreaturesPopulation::selectCreature() const {
	return creatures.at(randBetweenAndStepped(0.0, creatures.size(), 1.0));
}

void CreaturesPopulation::sexualCrossover(const Creature *mom,
		const Creature *dad, Creature *sister, Creature *brother) const {
	for (unsigned i = 0; i < getCreatureActivitiesCount(); i++) {
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

void CreaturesPopulation::reproduce() {
	LOG4CXX_TRACE(logger, "reproduce");
	// TODO - implementacja
	if (creatures.size() <= 1)
		return;
	const Creature *mom = selectCreature();
	const Creature *dad = selectCreature();
	Creature *child1 = new Creature(*mom);
	Creature *child2 = new Creature(*dad);

	if (flipCoin(CROSSOVER)) {
		sexualCrossover(mom, dad, child1, child2);
	}

	child1->mutate(MUTATION);
	child2->mutate(MUTATION);

	// Blokada zmian dla innych wątków
	polulationChange.lock();

	creatures.push_back(child1);
	creatures.push_back(child2);

	LOG4CXX_DEBUG(logger,
			"New child1 " << " T:" << child1 << " OLD:" << child1->getAge() << " GENE: " << *child1);
	LOG4CXX_DEBUG(logger,
			"New child2 " << " T:" << child2 << " OLD:" << child2->getAge() << " GENE: " << *child2);

	//ga->removeIndividual(creatureToRemove);
	while (creatures.size() > 30) {
		const Creature *dead = selectCreature();
		std::remove(creatures.begin(), creatures.end(), dead);
	}

	std::sort(creatures.begin(), creatures.end());

	Creatures::iterator i = creatures.begin();
	for (; i != creatures.end(); i++) {
		LOG4CXX_DEBUG(logger,
				" CREATURE " << " T:" << *i << " OLD:" << (*i)->getAge() << " GENE: " << *(*i));
	}
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

void CreaturesPopulation::remove(Creature *creatureToRemove) {
	LOG4CXX_TRACE(logger,
			" removing creature " << *creatureToRemove << " T:" << creatureToRemove << " TC:" << ((Creature *)creatureToRemove));
	polulationChange.lock();
	//ga->removeIndividual(creatureToRemove);
	LOG4CXX_DEBUG(logger,
			" population size before removing = " << creatures.size());

	Creatures::iterator toErase = std::find(creatures.begin(), creatures.end(),
			creatureToRemove);
	if (toErase != creatures.end())
		creatures.erase(toErase);

	LOG4CXX_DEBUG(logger,
			" population size after removing = " << creatures.size());
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

