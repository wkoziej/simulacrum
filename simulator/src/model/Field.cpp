/*
 * Field.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

//#include "stdlib.h"
#include <vector>
#include <QtCore/QSemaphore>

#include "Population.h"
#include "Field.h"
#include "World.h"
#include "Market.h"
#include "Types.h"

typedef std::vector<QSemaphore *> ArticleStocks;

using namespace std;

LoggerPtr Field::logger(Logger::getLogger("field"));

class FieldPrivate {
public:
	//float updateProductPrice(int i);
	//float updateResourcePrice(int i);
	PopulationsMap populations;
	Market * market;
	ArticleStocks stocks;
	UnsignedVector articleRenewal;
};

/*
 Field::Field() {
 prv = new FieldPrivate();
 //prv->resourceQuantities.assign(World::NO_OF_ARTICLES, 0);
 prv->stocks.assign(World::NO_OF_ARTICLES, )
 prv->articleRenewal.assign(World::NO_OF_ARTICLES, 0);
 prv->maxResourcesQuantities.assign(World::NO_OF_ARTICLES, 0);
 prv->maxProductsQuantities.assign(World::NO_OF_ARTICLES, 0);
 prv->resourcePriceCache.assign(World::NO_OF_ARTICLES, POSITIVE_INFINITY);
 prv->productPriceCache.assign(World::NO_OF_ARTICLES, POSITIVE_INFINITY);

 }
 */

Field::Field(const JSONObject &field) {
	prv = new FieldPrivate();
	prv->market = new Market();
	JSONArray articles = field.at(L"stocks")->AsArray();
	JSONArray::iterator article = articles.begin();

	prv->articleRenewal.assign(World::articles.size(), 0);
	for (unsigned c = 0; c < World::articles.size(); c++) {
		QSemaphore *stock = new QSemaphore();
		prv->stocks.push_back(stock);
	}

	for (; article != articles.end(); article++) {
		JSONObject properies = (*article)->AsObject();
		std::wstring articleName = properies.begin()->first;
		int articleIndex = World::getArticleIndex(articleName);
		assert(articleIndex >= 0 && articleIndex <World::articles.size());
		JSONObject articleStockProperty = properies.begin()->second->AsObject();
		int quant = 0;
		if (articleStockProperty.count(L"stock") > 0) {
			quant = articleStockProperty.at(L"stock")->AsNumber();
		}
		int reneval = 0;
		if (articleStockProperty.count(L"reneval") > 0) {
			reneval = articleStockProperty.at(L"reneval")->AsNumber();
		}
		prv->stocks.at(articleIndex)->release(quant);
		prv->articleRenewal.at(articleIndex) = reneval;
	}
	ArticleStocks::iterator q;
	UnsignedVector::iterator r;
	r = prv->articleRenewal.begin();
	q = prv->stocks.begin();
	for (; r != prv->articleRenewal.end(); r++, q++) {
		LOG4CXX_DEBUG(logger, "article [" << r - prv->articleRenewal.begin( ) << "] = [" << (*q)->available() << "," << *r <<"]");
	}
}

/*void Field::initializeRandomly() {
 LOG4CXX_TRACE(logger, "initializeRandomly");
 moveLag = (random() / (float) RAND_MAX) * MAX_MOVE_LAG;
 LOG4CXX_TRACE(logger, "moveLag: " << moveLag);
 resourceQuantities.assign(World::NO_OF_ARTICLES, 0);
 UnsignedVector::iterator f;
 UnsignedVector::iterator mP = maxProductsQuantities.begin();
 UnsignedVector::iterator mR = maxResourcesQuantities.begin();
 for (f = articleRenewal.begin(); f != articleRenewal.end(); f++, mP++, mR++) {
 *f = (random() / (float) RAND_MAX) * MAX_RENEWAL;
 *mP = (random() / (float) RAND_MAX) * MAX_PRODUCT_QUANT;
 *mR = (random() / (float) RAND_MAX) * MAX_RESOURCE_QUANT;
 LOG4CXX_DEBUG(logger, "resourceRenewal [" << f - articleRenewal.begin( ) << "]: " << *f);
 }
 }*/

void Field::renovateResources() {
	LOG4CXX_TRACE(logger, "renovateResources");
	UnsignedVector::iterator r = prv->articleRenewal.begin();
	ArticleStocks::iterator q = prv->stocks.begin();
	//UnsignedVector::iterator mR = maxResourcesQuantities.begin();
	for (; r != prv->articleRenewal.end(); r++, q++) {
		(*q)->release(*r);
		LOG4CXX_DEBUG(logger, "resourceQuantity [" << r - prv->articleRenewal.begin() << "]: "
				<< (*q)->available() << ", ren:" << *r );
	}

}
/*

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


 */

const Recipe *Field::getRecipe(unsigned articleId) const {
	// Jeżeli nie ma recepty związanej z polem to weź ogólnoświatową
	return World::recipes.at(articleId);
}

Market *Field::getMarket() {
	return prv->market;
}

bool Field::tryTakeArticle(unsigned articleId) {
	return prv->stocks.at(articleId)->tryAcquire();
}

unsigned Field::articleStock(unsigned articleId) {
	return prv->stocks.at(articleId)->available();
}

void Field::putArticle(unsigned articleId) {
	prv->stocks.at(articleId)->release();
}

void Field::addPopulation(CreaturesPopulation *population) {
	NamedPopulation namedPopulation(population->getName(), population);
	prv->populations.insert(namedPopulation);
}

CreaturesPopulation *Field::getPopulation(std::wstring name) {
	PopulationsMap::iterator i = prv->populations.find(name);
	if (i != prv->populations.end()) {
		return i->second;
	}
	return NULL;
}

std::list<CreaturesPopulation *> Field::getPopulations() {
	PopulationsMap::iterator i = prv->populations.begin();
	std::list<CreaturesPopulation *> populationList;
	for (; i != prv->populations.end(); i++) {
		populationList.push_back(i->second);
	}
	return populationList;
}
/*
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
 */

/*void Field::increaseProductQuantity(unsigned productIndex,
 float productQuantity) {
 LOG4CXX_DEBUG(logger, "productQuantity [" << productIndex << "] incresed from " << productsQuantities.at(productIndex) << " to " << productsQuantities.at(productIndex) + productQuantity);
 productsQuantities.at(productIndex) += productQuantity;
 if ( productsQuantities.at(productIndex) > maxProductsQuantities.at(productIndex) ) {
 productsQuantities.at(productIndex) = maxProductsQuantities.at(productIndex);
 }
 }*/

void Field::decreaseArticleQuantity(unsigned resourceIndex, float resourceUsed) {
	LOG4CXX_DEBUG(logger, "resourceQuantities [" << resourceIndex << "] decreased from "
			<< prv->stocks.at(resourceIndex)->available() << " to " << prv->stocks.at(resourceIndex)->available() - resourceUsed);
	prv->stocks.at(resourceIndex)->acquire(resourceUsed);
	assert(prv->stocks.at(resourceIndex)->available() >= 0);
}
/*
 float Field::getProductQuantity(unsigned productIndex) {
 return productsQuantities.at(productIndex);
 }*/

/*void Field::decreaseProductQuantity(float productEaten, unsigned productIndex) {
 productsQuantities.at(productIndex) -= productEaten;
 }*/

/*
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
 */

Field::~Field() {
	delete prv->market;
	delete prv;
	ArticleStocks::iterator q = prv->stocks.begin();
	for (; q != prv->stocks.end(); q++) {
		delete *q;
	}
}
