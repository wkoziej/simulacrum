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
typedef std::pair<std::wstring, CreaturesPopulation *> NamedPopulation;
typedef std::map<std::wstring, CreaturesPopulation *> PopulationsMap;

class Field {
public:
	Field();
	Field(const JSONObject &JSONfield);
	void initializeRandomly();
	void renovateResources();
	float getResourceQuantity(unsigned index) const ;
	void decreaseResourceQuantity(unsigned resourceIndex, float resourceUsed);
	void decreaseProductQuantity(float productEaten, unsigned productIndex);
	bool getOut (float &velocity);
	virtual ~Field();
	const FloatVector &resourceQuantitiesVector () const {return resourceQuantities; }
	float productStock(int i) const ;
	float productNeeds(int i) const ;
	float resourceNeeds(int i) const;
	float productPrice(int i) const;
	float resourcePrice(int i) const;
	float updateProductPrice(int i);
	float updateResourcePrice(int i);
	PopulationsMap populations;
private:
	float moveLag; // Opóźnienie ruchu
	FloatVector resourceQuantities;
	FloatVector resourceRenewal;
	//FloatVector productsQuantities;
	FloatVector maxResourcesQuantities;
	FloatVector maxProductsQuantities;
	FloatVector resourcePriceCache;
	FloatVector	productPriceCache;
	// Logowanie
	static LoggerPtr logger;
};

#endif /* FIELD_H_ */
