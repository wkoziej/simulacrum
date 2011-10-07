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

#include "Config.h"

using namespace std;
using namespace log4cxx;

typedef std::vector<float> FloatVector;

class Field {
public:
	Field();
	void initializeRandomly();
	void renovateResources();
	float getResourceQuantity(unsigned index);
	//void increaseProductQuantity(unsigned productIndex, float productQuantity);
	void decreaseResourceQuantity(unsigned resourceIndex, float resourceUsed);
	//float getProductQuantity(unsigned productIndex);
	void decreaseProductQuantity(float productEaten, unsigned productIndex);
	bool getOut (float &velocity);
	virtual ~Field();
	const FloatVector &resourceQuantitiesVector () const {return resourceQuantities; }
//	const FloatVector &productsQuantitiesVector () const { return productsQuantities; }

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
