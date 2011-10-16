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
	resourceQuantities.assign(World::NO_OF_ARTICLES, 0);
	articleRenewal.assign(World::NO_OF_ARTICLES, 0);
	maxResourcesQuantities.assign(World::NO_OF_ARTICLES, 0);
	maxProductsQuantities.assign(World::NO_OF_ARTICLES, 0);
	resourcePriceCache.assign(World::NO_OF_ARTICLES, POSITIVE_INFINITY);
	productPriceCache.assign(World::NO_OF_ARTICLES, POSITIVE_INFINITY);

}

Field::Field(const JSONObject &field) {
	JSONArray resources = field.at(L"resources")->AsArray();
	assert (resources.size() == World::NO_OF_ARTICLES);
	JSONArray::iterator resource = resources.begin();
	for (; resource != resources.end(); resource++) {
		JSONArray properies = (*resource)->AsArray();
		resourceQuantities.push_back(properies.at(0)->AsNumber());
		articleRenewal.push_back(properies.at(1)->AsNumber());
	}
	resourcePriceCache.assign(World::NO_OF_ARTICLES, POSITIVE_INFINITY);
	productPriceCache.assign(World::NO_OF_ARTICLES, POSITIVE_INFINITY);
	maxResourcesQuantities.assign(World::NO_OF_ARTICLES, POSITIVE_INFINITY);
	maxProductsQuantities.assign(World::NO_OF_ARTICLES, POSITIVE_INFINITY);
	FloatVector::iterator q;
	FloatVector::iterator r;
	r = articleRenewal.begin();
	q = resourceQuantities.begin();
	for (; r != articleRenewal.end(); r++, q++) {
		LOG4CXX_DEBUG(logger, "resource [" << r - articleRenewal.begin( ) << "] = [" << *q << "," << *r <<"]");
	}
}

void Field::initializeRandomly() {
	LOG4CXX_TRACE(logger, "initializeRandomly");
	moveLag = (random() / (float) RAND_MAX) * MAX_MOVE_LAG;
	LOG4CXX_TRACE(logger, "moveLag: " << moveLag);
	resourceQuantities.assign(World::NO_OF_ARTICLES, 0);
	FloatVector::iterator f;
	FloatVector::iterator mP = maxProductsQuantities.begin();
	FloatVector::iterator mR = maxResourcesQuantities.begin();
	for (f = articleRenewal.begin(); f != articleRenewal.end(); f++, mP++, mR++) {
		*f = (random() / (float) RAND_MAX) * MAX_RENEWAL;
		*mP = (random() / (float) RAND_MAX) * MAX_PRODUCT_QUANT;
		*mR = (random() / (float) RAND_MAX) * MAX_RESOURCE_QUANT;
		LOG4CXX_DEBUG(logger, "resourceRenewal [" << f - articleRenewal.begin( ) << "]: " << *f);
	}
}

void Field::renovateResources() {
	LOG4CXX_TRACE(logger, "renovateResources");
	FloatVector::iterator r = articleRenewal.begin();
	FloatVector::iterator q = resourceQuantities.begin();
	FloatVector::iterator mR = maxResourcesQuantities.begin();
	for (; r != articleRenewal.end(); r++, q++, mR++) {
		*q += *r;
		if (*q > *mR) {
			*q = *mR;
		}
		LOG4CXX_DEBUG(logger, "resourceQuantity [" << r - articleRenewal.begin() << "]: " << *q << ", ren:" << *r << ", maxRes : " << *mR);
	}

}

float Field::getArticleQuantity(unsigned index) const {
	return resourceQuantities.at(index);
}

float Field::productStock(int i) const {
	float kept = 0.0;
	PopulationsMap::const_iterator population = populations.begin();
	for (; population != populations.end(); population++) {
		kept += population->second->keptProductSum(i);
	}
	return kept;
}

float Field::productNeeds(int i) const {
	float needs = 0.0;
	PopulationsMap::const_iterator population = populations.begin();
	for (; population != populations.end(); population++) {
		needs += population->second->productNeeds(i);
	}
	return needs;
}

float Field::resourceNeeds(int i) const {
	PopulationsMap::const_iterator population = populations.begin();
	float needs = 0.0;
	for (; population != populations.end(); population++) {
		needs += population->second->resourceNeeds(i);
	}
	return needs;
}

float Field::productPrice(int i) const {
	// Wartość wytworzonych produktów
	float price = productPriceCache.at(i);
	return price;
}

float Field::resourcePrice(int i) const {
	float price = resourcePriceCache.at(i);
	return price;
}

float Field::updateProductPrice(int i) {
	float price = productPriceCache.at(i);
	// Jaka ilosc produktu jest dostepna w zapasach populacji
	float kept = productStock(i);
	float needs = productNeeds(i);
	if (kept > 0)
		price = needs / kept;
	LOG4CXX_DEBUG(logger, "product [" << i << "]: kept = " << kept << ", needs = " << needs << ", price = " << price);
	productPriceCache.at(i) = price;
	return price;
}

float Field::updateResourcePrice(int i) {
	float price = resourcePriceCache.at(i);
	// Jaka ilosc produktu jest dostepna w zapasach populacji
	PopulationsMap::const_iterator population = populations.begin();
	float availabble = getArticleQuantity(i);
	float needs = resourceNeeds(i);
	LOG4CXX_DEBUG(logger, "resource [" << i << "]: available = " << availabble << ", needs = " << needs);
	if (availabble > 0)
		price = needs / availabble;
	resourcePriceCache.at(i) = price;
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

void Field::decreaseArticleQuantity(unsigned resourceIndex, float resourceUsed) {
	LOG4CXX_DEBUG(logger, "resourceQuantities [" << resourceIndex << "] decreased from " <<resourceQuantities.at(resourceIndex) << " to " << resourceQuantities.at(resourceIndex) - resourceUsed);
	resourceQuantities.at(resourceIndex) -= resourceUsed;
	assert(resourceQuantities.at(resourceIndex) >= 0);
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
