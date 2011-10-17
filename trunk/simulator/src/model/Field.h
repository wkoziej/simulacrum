/*
 * Field.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#ifndef FIELD_H_
#define FIELD_H_


//#include <QtCore/QSemaphore>
#include "log4cxx/logger.h"
#include "JSON/JSON.h"
#include "Config.h"

using namespace log4cxx;

class FieldPrivate;
class Recipe;
class Market;

class Field {
public:
	//Field();
	Field(const JSONObject &JSONfield);
	//void initializeRandomly();
	void renovateResources();
	//float getArticleQuantity(unsigned index) const ;
	void decreaseArticleQuantity(unsigned resourceIndex, float resourceUsed);
	//void decreaseProductQuantity(float productEaten, unsigned productIndex);
	//bool getOut (float &velocity);
	virtual ~Field();
	//const FloatVector &resourceQuantitiesVector () const {return resourceQuantities; }
	//float productStock(int i) const ;
	//float productNeeds(int i) const ;
	//float resourceNeeds(int i) const;
	//float productPrice(unsigned  i) const;
	//float resourcePrice(unsigned  i) const;
	const Recipe *getRecipe(unsigned articleId) const;
	Market *getMarket ();
	bool tryTakeArticle (unsigned articleId);
	void putArticle (unsigned articleId);
	//float getMoveLag () const { return moveLag; }
private:
	FieldPrivate *prv;
	//float moveLag; // Opóźnienie ruchu
	//FloatVector resourceQuantities;
	//FloatVector productsQuantities;
	//FloatVector maxResourcesQuantities;
	//FloatVector maxProductsQuantities;
	//FloatVector resourcePriceCache;
	//FloatVector	productPriceCache;
	// Logowanie
	static LoggerPtr logger;
};

#endif /* FIELD_H_ */
