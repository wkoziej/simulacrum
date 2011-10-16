/*
 * Recipe.h
 *
 *  Created on: 16-10-2011
 *      Author: wojtas
 */

#ifndef RECIPE_H_
#define RECIPE_H_

class Recipe {
public:
	Recipe();
	virtual ~Recipe();
	std::vector<float> getIngredientsVector() const;
};

#endif /* RECIPE_H_ */
