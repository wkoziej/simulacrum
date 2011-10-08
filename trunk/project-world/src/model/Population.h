/*
 * Population.h
 *
 *  Created on: 2011-10-07
 *      Author: wojtas
 */

#ifndef POPULATION_H_
#define POPULATION_H_

#include <ga/GAPopulation.h>
#include "Field.h"
#include "Creature.h"

class Population: public GAPopulation {
public:
	Population(Field *field);
	virtual ~Population();
	float productPrice(int i);
	float resourcePrice(int i);
	float productNeeds(int i);
	float resourceNeeds(int i);
	float objectiveAvarage ();
	//	float neededProductSum (int i);
	float keptProductSum(int i) {
		return productsStock.at(i);
	}
	;
private:
	Field *field;
	std::vector<float> productsStock;
};

class CreaturesOnFieldVisitor {
public:
	CreaturesOnFieldVisitor() {
	}
	;
	virtual void visit(Creature *creature, Field *field, Population *,
			unsigned x, unsigned y) = 0;
	virtual ~CreaturesOnFieldVisitor() {
	}
	;
};

class PopulationOnFieldVisitor {
public:
	PopulationOnFieldVisitor() {
	}
	;
	virtual void visit(Population *population, Field *field, unsigned x,
			unsigned y) = 0;
	virtual ~PopulationOnFieldVisitor() {
	}
	;
protected:
};

class CreaturesOfPopulationOnFieldVisitor : public PopulationOnFieldVisitor
{
private:
	CreaturesOnFieldVisitor *visitor;
	// Logowanie
	static LoggerPtr logger;
public:
	CreaturesOfPopulationOnFieldVisitor(CreaturesOnFieldVisitor *v);
	virtual void visit(Population *population, Field *field, unsigned x,
			unsigned y);
};

#endif /* POPULATION_H_ */
