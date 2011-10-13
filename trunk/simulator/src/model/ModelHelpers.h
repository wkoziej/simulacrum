/*
 * ModelHelpers.h
 *
 *  Created on: 2011-10-09
 *      Author: wojtas
 */

#ifndef MODELHELPERS_H_
#define MODELHELPERS_H_

#include "Creature.h"
#include "Field.h"
#include "Population.h"

class CreaturesOnFieldVisitor {
public:
	CreaturesOnFieldVisitor() {
	}
	;
	virtual void visit(Creature *creature, Field *field, CreaturesPopulation *,
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
	virtual void visit(CreaturesPopulation *population, Field *field, unsigned x,
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
	virtual void visit(CreaturesPopulation *population, Field *field, unsigned x,
			unsigned y);
};

class FieldsVisitor {
public:
	virtual void visit(Field *field) = 0;
};


#endif /* MODELHELPERS_H_ */
