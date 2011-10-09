/*
 * ModelHelpers.cpp
 *
 *  Created on: 2011-10-09
 *      Author: wojtas
 */

#include "ModelHelpers.h"

class UpdateValueVisitor: public CreaturesOnFieldVisitor {
protected:
	float value;
private:

	int productIndex;
public:
	UpdateValueVisitor() :
		value(0.0) {
	}

	float getValue() {
		return value;
	}

	void visit(Creature *creature, Field *field, Population *p, unsigned x,
			unsigned y) {
		updateValue(creature, field, p, x, y);
	}

	virtual void updateValue(Creature *creature, Field *field, Population *p,
			unsigned x, unsigned y) = 0;
};

class NeededProductVisitor: public UpdateValueVisitor {
private:
	int index;
public:
	NeededProductVisitor(int productIndex) {
		this->index = productIndex;
	}

	void updateValue(Creature *creature, Field *field, Population *p,
			unsigned x, unsigned y) {
		value += creature->getNeedOfProductRatio(index);
	}
};

class NeededResourceVisitor: public UpdateValueVisitor {
private:
	int index;
public:
	NeededResourceVisitor(int productIndex) {
		this->index = productIndex;
	}

	void updateValue(Creature *creature, Field *field, Population *p,
			unsigned x, unsigned y) {
		value += creature->getNeedOfResource(index);
	}
};

class SumObjectiveVisitor: public UpdateValueVisitor {
public:
	void updateValue(Creature *creature, Field *field, Population *p,
			unsigned x, unsigned y) {
		value += creature->getFenotype()->objectiveValue;
	}
};
