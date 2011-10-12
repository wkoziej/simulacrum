/*
 * Population.h
 *
 *  Created on: 2011-10-07
 *      Author: wojtas
 */

#ifndef POPULATION_H_
#define POPULATION_H_
#include "JSON/JSONValue.h"
#include <ga/GAPopulation.h>
#include <log4cxx/logger.h>
class Field;
using namespace log4cxx;

class CreaturesPopulation: public GAPopulation {
public:
	CreaturesPopulation();
	CreaturesPopulation(const Field *field, const JSONObject &population);
	virtual ~CreaturesPopulation();
	float productNeeds(int i);
	float resourceNeeds(int i);
	float objectiveAvarage ();
	std::wstring getName () { return name; };
	float keptProductSum(int i);
	void updateProductStock (int i, float delta);
	void loadCreatures (const JSONObject &population);
	int getCreaturesTalentsCount () const { return creaturesTalentsCount; }
	int getCreaturesNeedsCount () const { return creaturesNeedsCount; }
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
