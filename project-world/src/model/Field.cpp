/*
 * Field.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#include "stdlib.h"

#include "Field.h"
#include "World.h"

LoggerPtr Field::logger(Logger::getLogger("field"));

Field::Field() {
	resourceQuantities.assign(World::NO_OF_RESOURCES, 0);
	resourceRenewal.assign(World::NO_OF_RESOURCES, 0);
	maxResourcesQuantities.assign(World::NO_OF_RESOURCES, 0);
	maxProductsQuantities.assign(World::NO_OF_RESOURCES, 0);
}

Field::Field(const JSONObject &field) {
	JSONArray resources = field.at(L"resources")->AsArray();
	assert (resources.size() == World::NO_OF_RESOURCES);
	JSONArray::iterator resource = resources.begin();
	for (; resource != resources.end(); resource++) {
		JSONArray properies = (*resource)->AsArray();
		resourceQuantities.push_back(properies.at(0)->AsNumber());
		resourceRenewal.push_back(properies.at(1)->AsNumber());
	}
	FloatVector::iterator q;
	FloatVector::iterator r;
	r = resourceRenewal.begin();
	q = resourceQuantities.begin();
	for (; r != resourceRenewal.end(); r++, q++) {
		LOG4CXX_DEBUG(logger, "resource [" << r - resourceRenewal.begin( ) << "] = [" << *q << "," << *r <<"]");
	}
}

void Field::initializeRandomly() {
	LOG4CXX_TRACE(logger, "initializeRandomly");
	moveLag = (random() / (float) RAND_MAX) * MAX_MOVE_LAG;
	LOG4CXX_TRACE(logger, "moveLag: " << moveLag);
	resourceQuantities.assign(World::NO_OF_RESOURCES, 0);
	FloatVector::iterator f;
	FloatVector::iterator mP = maxProductsQuantities.begin();
	FloatVector::iterator mR = maxResourcesQuantities.begin();
	for (f = resourceRenewal.begin(); f != resourceRenewal.end(); f++, mP++, mR++) {
		*f = (random() / (float) RAND_MAX) * MAX_RENEWAL;
		*mP = (random() / (float) RAND_MAX) * MAX_PRODUCT_QUANT;
		*mR = (random() / (float) RAND_MAX) * MAX_RESOURCE_QUANT;
		LOG4CXX_DEBUG(logger, "resourceRenewal [" << f - resourceRenewal.begin( ) << "]: " << *f);
	}
}

void Field::renovateResources() {
	LOG4CXX_TRACE(logger, "renovateResources");
	FloatVector::iterator r = resourceRenewal.begin();
	FloatVector::iterator q = resourceQuantities.begin();
	FloatVector::iterator mR = maxResourcesQuantities.begin();
	for (; r != resourceRenewal.end(); r++, q++, mR++) {
		*q += *r;
		if (*q > *mR) {
			*q = *mR;
		}
		LOG4CXX_DEBUG(logger, "resourceQuantity [" << r - resourceRenewal.begin() << "]: " << *q << ", ren:" << *r << ", maxRen : " << *mR);
	}

}

float Field::getResourceQuantity(unsigned index) {
	return resourceQuantities.at(index);
}

float Field::productPrice(int i) {
	// Wartość wytworzonych produktów
	float price = POSITIVE_INFINITY;
	// Jaka ilosc produktu jest dostepna w zapasach populacji
	PopulationsMap::iterator population = populations.begin();
	float kept = 0.0;
	float needs = 0.0;
	for (; population != populations.end(); population++) {
		kept += population->second->keptProductSum(i);
		needs += population->second->productNeeds(i);
	}
	LOG4CXX_DEBUG(logger, "product [" << i << "]: kept = " << kept << ", needs = " << needs);
	if (kept > 0)
		price = needs / kept;
	return price;
}

float Field::resourcePrice(int i) {
	float price = POSITIVE_INFINITY;
	// Jaka ilosc produktu jest dostepna w zapasach populacji
	PopulationsMap::iterator population = populations.begin();
	float availabble = getResourceQuantity(i);
	float needs = 0.0;
	for (; population != populations.end(); population++) {
		needs += population->second->resourceNeeds(i);
	}
	LOG4CXX_DEBUG(logger, "resource [" << i << "]: available = " << availabble << ", needs = " << needs);
	if (availabble > 0)
		price = needs / availabble;
	return price;
}

/*void Field::increaseProductQuantity(unsigned productIndex,
 float productQuantity) {
 LOG4CXX_DEBUG(logger, "productQuantity [" << productIndex << "] incresed from " << productsQuantities.at(productIndex) << " to " << productsQuantities.at(productIndex) + productQuantity);
 productsQuantities.at(productIndex) += productQuantity;
 if ( productsQuantities.at(productIndex) > maxProductsQuantities.at(productIndex) ) {
 productsQuantities.at(productIndex) = maxProductsQuantities.at(productIndex);
 }
 }*/

void Field::decreaseResourceQuantity(unsigned resourceIndex, float resourceUsed) {
	LOG4CXX_DEBUG(logger, "resourceQuantities [" << resourceIndex << "] decreased from " <<resourceQuantities.at(resourceIndex) << " to " << resourceQuantities.at(resourceIndex) - resourceUsed);
	resourceQuantities.at(resourceIndex) -= resourceUsed;
}
/*
 float Field::getProductQuantity(unsigned productIndex) {
 return productsQuantities.at(productIndex);
 }*/

/*void Field::decreaseProductQuantity(float productEaten, unsigned productIndex) {
 productsQuantities.at(productIndex) -= productEaten;
 }*/

bool Field::getOut(float &velocity) {
	bool isOut = false;
	LOG4CXX_DEBUG(logger, "velocity " <<velocity << ", moveLag: " << moveLag);
	if (moveLag > velocity) {
		velocity = 0;
	} else {
		isOut = true;
		velocity -= moveLag;
	}
	LOG4CXX_DEBUG(logger, " isOut:" << isOut);
	return isOut;
}

Field::~Field() {
	// TODO Auto-generated destructor stub
}
