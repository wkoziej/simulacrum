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
	//productsQuantities.assign(NO_OF_RESOURCES, 0);
	maxResourcesQuantities.assign(World::NO_OF_RESOURCES, 0);
	maxProductsQuantities.assign(World::NO_OF_RESOURCES, 0);
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
	//productsQuantities.assign(NO_OF_RESOURCES, 0);
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
