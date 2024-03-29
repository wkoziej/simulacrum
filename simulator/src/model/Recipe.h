/*
 * Recipe.h
 *
 *  Created on: 16-10-2011
 *      Author: wojtas
 */

#ifndef RECIPE_H_
#define RECIPE_H_
#include "Types.h"
class Recipe {
public:
	Recipe();
	virtual ~Recipe();
	UnsignedVector getIngredientsVector() const;
	void setIngredientsVector(const UnsignedVector &i);
private:
	UnsignedVector ingredients;
};

#endif /* RECIPE_H_ */
