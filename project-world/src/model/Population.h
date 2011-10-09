/*
 * Population.h
 *
 *  Created on: 2011-10-07
 *      Author: wojtas
 */

#ifndef POPULATION_H_
#define POPULATION_H_
#include "JSON/JSON.h"
#include <ga/GAPopulation.h>
//#include "Field.h"


class Population: public GAPopulation {
public:
	Population();
	Population(const JSONObject &population);
	virtual ~Population();
	float productNeeds(int i);
	float resourceNeeds(int i);
	float objectiveAvarage ();
	std::wstring getName () { return name; };
	//	float neededProductSum (int i);
	float keptProductSum(int i) {
		return productsStock.at(i);
	}
	;
	void loadCreatures (const JSONObject &population);
	int getCreaturesTalentsCount () { return creaturesTalentsCount; };
	int getCreaturesNeedsCount () { return creaturesNeedsCount; };
private:
	//Field *field;
	std::vector<float> productsStock;
	std::wstring name;
	int creaturesTalentsCount;
	int creaturesNeedsCount;
	// Logowanie
	static LoggerPtr logger;
};

#endif /* POPULATION_H_ */
