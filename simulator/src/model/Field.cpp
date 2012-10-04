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

Field::Field() {
	initializeEmptyField();
}

Field::Field(const JSONObject &field) {
	initializeEmptyField();
	JSONArray articles = field.at(L"stocks")->AsArray();
	JSONArray::iterator article = articles.begin();
	for (; article != articles.end(); article++) {
		JSONObject properies = (*article)->AsObject();
		std::wstring articleName = properies.begin()->first;
		unsigned articleIndex = World::getArticleIndex(articleName);
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

void Field::initializeEmptyField() {
	prv = new FieldPrivate();
	prv->market = new Market();
	prv->articleRenewal.assign(World::articles.size(), 0);
	for (unsigned c = 0; c < World::articles.size(); c++) {
		QSemaphore *stock = new QSemaphore();
		prv->stocks.push_back(stock);
	}
}

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

const Recipe *Field::getRecipe(unsigned articleId) const {
	// Jeżeli nie ma recepty związanej z polem to weź ogólnoświatową
	return World::recipes.at(articleId);
}

Market *Field::getMarket() {
	return prv->market;
}

bool Field::tryTakeArticle(unsigned articleId) {
	LOG4CXX_DEBUG(logger, "trying to take article " << articleId << " from field " << this);
	return prv->stocks.at(articleId)->tryAcquire();
}

unsigned Field::articleStock(unsigned articleId) {
	LOG4CXX_DEBUG(logger, " checking stock of article " << articleId << " on field " << this);
	return prv->stocks.at(articleId)->available();
}

void Field::putArticle(unsigned articleId, unsigned quantity) {
	assert (quantity < 10000);
	LOG4CXX_DEBUG(logger, " putting article " << articleId << " in field " << this);
	prv->stocks.at(articleId)->release(quantity);
}

void Field::addPopulation(CreaturesPopulation *population) {
	NamedPopulation namedPopulation(population->getName(), population);
	prv->populations.insert(namedPopulation);
}

CreaturesPopulation *Field::getPopulation(std::wstring name) {
	LOG4CXX_DEBUG(logger, " searching population " << wstring2string(name));
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

void Field::decreaseArticleQuantity(unsigned resourceIndex, float resourceUsed) {
	LOG4CXX_DEBUG(logger, "resourceQuantities [" << resourceIndex << "] decreased from "
			<< prv->stocks.at(resourceIndex)->available() << " to " << prv->stocks.at(resourceIndex)->available() - resourceUsed << " on field " << this);
	prv->stocks.at(resourceIndex)->acquire(resourceUsed);
	assert(prv->stocks.at(resourceIndex)->available() >= 0);
}

Field::~Field() {
	delete prv->market;
	delete prv;
	ArticleStocks::iterator q = prv->stocks.begin();
	for (; q != prv->stocks.end(); q++) {
		delete *q;
	}
}
