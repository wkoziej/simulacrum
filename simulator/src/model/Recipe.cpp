/*
 * Recipe.cpp
 *
 *  Created on: 16-10-2011
 *      Author: wojtas
 */

#include "Recipe.h"

Recipe::Recipe() {
}

Recipe::~Recipe() {
}

UnsignedVector Recipe::getIngredientsVector() const {
	return ingredients;
}

void Recipe::setIngredientsVector(const UnsignedVector &i) {
	ingredients = i;
}
