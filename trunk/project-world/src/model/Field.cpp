/*
 * Field.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#include "stdlib.h"

#include "Field.h"

LoggerPtr Field::logger(Logger::getLogger("field"));

Field::Field() {
	resourceQuantities.assign(NO_OF_RESOURCES, 0);
	resourceRenewal.assign(NO_OF_RESOURCES, 0);
	productsQuantities.assign(NO_OF_RESOURCES, 0);

}

void Field::initializeRandomly() {
	LOG4CXX_TRACE(logger, "initializeRandomly");
	moveLag = (random() / (float) RAND_MAX) * MAX_MOVE_LAG;
	LOG4CXX_TRACE(logger, "moveLag: " << moveLag);
	resourceQuantities.assign(NO_OF_RESOURCES, 0);
	FloatVector::iterator f;
	for (f = resourceRenewal.begin(); f != resourceRenewal.end(); f++) {
		*f = (random() / (float) RAND_MAX) * MAX_RENEWAL;
		LOG4CXX_DEBUG(logger, "resourceRenewal [" << f - resourceRenewal.begin( ) << "]: " << *f);
	}
	productsQuantities.assign(NO_OF_RESOURCES, 0);
}

void Field::renovateResources() {
	LOG4CXX_TRACE(logger, "renovateResources");
	FloatVector::iterator r = resourceRenewal.begin();
	FloatVector::iterator q = resourceQuantities.begin();
	for (; r != resourceRenewal.end(); r++, q++) {
		*q += *r;
		LOG4CXX_DEBUG(logger, "resourceQuantity [" << r - resourceRenewal.begin() << "]: " << *q);
	}
}

float Field::getResourceQuantity(unsigned index) {
	return resourceQuantities.at(index);
}

void Field::increaseProductQuantity(unsigned productIndex,
		float productQuantity) {
	LOG4CXX_DEBUG(logger, "productQuantity [" << productIndex << "] incresed from " << productsQuantities.at(productIndex) << " to " << productsQuantities.at(productIndex) + productQuantity);
	productsQuantities.at(productIndex) += productQuantity;
}

void Field::decreaseResourceQuantity(unsigned resourceIndex, float resourceUsed) {
	LOG4CXX_DEBUG(logger, "resourceQuantities [" << resourceIndex << "] decreased from " <<resourceQuantities.at(resourceIndex) << " to " << 	resourceQuantities.at(resourceIndex) - resourceUsed);
	resourceQuantities.at(resourceIndex) -= resourceUsed;
}

float Field::getProductQuantity(unsigned productIndex) {
	return productsQuantities.at(productIndex);
}

void Field::decreaseProductQuantity(float productEaten, unsigned productIndex) {
	productsQuantities.at(productIndex) -= productEaten;
}

bool Field::getOut (float &velocity) {
	bool isOut = false;
	LOG4CXX_DEBUG(logger, "velocity " <<velocity  << ", moveLag: " << moveLag);
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
