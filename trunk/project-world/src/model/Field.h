/*
 * Field.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#ifndef FIELD_H_
#define FIELD_H_

#include <vector>
#include "log4cxx/logger.h"
#include "JSON/JSON.h"

#include "Config.h"
#include "Population.h"

using namespace std;
using namespace log4cxx;

typedef std::vector<float> FloatVector;
typedef std::pair<std::wstring, Population *> NamedPopulation;
typedef std::map<std::wstring, Population *> PopulationsMap;

class Field {
public:
	Field();
	Field(const JSONObject &JSONfield);
	void initializeRandomly();
	void renovateResources();
	float getResourceQuantity(unsigned index);
	void decreaseResourceQuantity(unsigned resourceIndex, float resourceUsed);
	void decreaseProductQuantity(float productEaten, unsigned productIndex);
	bool getOut (float &velocity);
	virtual ~Field();
	const FloatVector &resourceQuantitiesVector () const {return resourceQuantities; }
	float productPrice(int i);
	float resourcePrice(int i);
	PopulationsMap populations;
private:
	float moveLag; // Opóźnienie ruchu
	FloatVector resourceQuantities;
	FloatVector resourceRenewal;
	//FloatVector productsQuantities;
	FloatVector maxResourcesQuantities;
	FloatVector maxProductsQuantities;
	// Logowanie
	static LoggerPtr logger;
};

#endif /* FIELD_H_ */
