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
	CreaturesPopulation(const CreaturesPopulation *species);
	CreaturesPopulation(Field *field, const JSONObject &population);
	virtual ~CreaturesPopulation();
	//float productNeeds(int i);
	//float resourceNeeds(int i);
	float objectiveAvarage();
	std::wstring getName() const {
		return name;
	}
	;
	//float keptProductSum(int i);
	//void updateProductStock (int i, float delta);
	void loadCreatures(const JSONObject &population);
	unsigned get0ArgActivitiesRoom() const;
	unsigned get1ArgActivitiesRoom() const;
	unsigned get2ArgActivitiesRoom() const;

	//int getCreaturesTalentsCount () const { return creaturesTalentsCount; }
	//int getCreaturesNeedsCount () const { return creaturesNeedsCount; }
private:
	//Field *field;
	//std::vector<float> productsStock;
	std::wstring name;
	unsigned _0ArgActivityRoom;
	unsigned _1ArgActivityRoom;
	unsigned _2ArgActivityRoom;
	//int creaturesTalentsCount;
	//int creaturesNeedsCount;
	// Logowanie
	static LoggerPtr logger;
};

typedef std::pair<std::wstring, CreaturesPopulation *> NamedPopulation;
typedef std::map<std::wstring, CreaturesPopulation *> PopulationsMap;

#endif /* POPULATION_H_ */
