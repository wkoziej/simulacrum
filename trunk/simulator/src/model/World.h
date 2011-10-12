/*
 * World.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
//#include <list>
#include <log4cxx/logger.h>

#include "Field.h"
#include "ModelHelpers.h"

//using namespace std;
//using namespace log4cxx;

typedef std::vector<Field *> FieldsVector;
typedef std::vector<FieldsVector> FieldsMatrix;
//typedef std::list<Creature *> CreaturesList;
//typedef std::vector<PopulationsList> PopulationsListVector;
//typedef std::vector<PopulationsListVector> PopulationsMatrix;

class World  {

public:
	static World *createRandomWorld(unsigned X, unsigned Y);
	static World *readWorldFromFile(const char *fileName);

	static World *getWorld() {
		if (singleton == NULL)
			singleton = new World();
		return singleton;
	}
	;
	virtual ~World();
	void initializeRandomly();
	void createRandomCreatures();
	void creaturesWorking();
	void creaturesSupplying();
	void creaturesDying();
	void creaturesReproducting();
	void creaturesMoving();
	void nextYear();
	bool creaturesExists();
	void step();
	void countResourcesPrices() ;
	void countProductsPrices() ;

	CreaturesPopulation *findPopulationOnField (std::wstring name, int x,  int y);
	FieldsMatrix fields;
	//PopulationsMatrix populations;
	void iteratePopulationOnFields(PopulationOnFieldVisitor *visitor);
	void iterateCreaturesOnFields(CreaturesOnFieldVisitor *visitor);
	static int NO_OF_RESOURCES; //3 // Liczba surowców -> 0 - natchnienie, 1 - materia
	static int NO_OF_PRODUCTS; //3 // Liczba surowców -> 0 - natchnienie, 1 - materia

private:
	static World *singleton;
	World() {
		if (singleton == NULL)
			singleton = this;
	}
	// Logowanie
	static LoggerPtr logger;
	void createFieldsAndPopulations(unsigned X, unsigned Y);

};

#endif /* WORLD_H_ */
