/*
 * Market.cpp
 *
 *  Created on: 16-10-2011
 *      Author: wojtas
 */

#include "Market.h"

Market::Market() {
	// TODO Auto-generated constructor stub
	stocks.assign(World::NO_OF_ARTICLES, QSemaphore());
	queries.assign(World::NO_OF_ARTICLES, ClientVisits());
	locks.assign(World::NO_OF_ARTICLES, QMutex());
	articleSellPricesHistory.assign(World::NO_OF_ARTICLES, 0);
	articleBuyPricesHistory.assign(World::NO_OF_ARTICLES, 0);
}

Market::~Market() {
	// TODO Auto-generated destructor stub
}

bool Market::isArticleAvailable(QString clientId, unsigned articleId,
		float articleQuant) {
	rememberClientQuery(clientId);
	return stocks.at(articleId).available() > articleQuant;
}

float Market::articleSellPrice(QString clientId, unsigned articleId) {
	float sellPrice = 0.0;
	float buyPrice = 0.0;
	// Zapisz zainteresowanie klienta -> wpływ na cenę sprzedaży
	rememberClientQuery(clientId);
	getPrices(clientId, articleId, sellPrice, buyPrice);
	return sellPrice;
}

float Market::articleBuyPrice(QString clientId, unsigned articleId) {
	float sellPrice = 0.0;
	float buyPrice = 0.0;
	getPrices(clientId, articleId, sellPrice, buyPrice);
	return buyPrice;
}

void Market::getPrices(QString clientId, unsigned articleId, float &sellPrice,
		float &buyPrice) {
	int availableQuant = stocks.at(articleId).available();
	QMutex *articleQuery = &locks.at(articleId);
	articleQuery->lock();
	int queryCount = queries.at(articleId).size();
	if (availableQuant > 0) {
		buyPrice = calculateBuyPrice(articleId, availableQuant, queryCount);
		sellPrice = calculateSellPrice(articleId, availableQuant, queryCount);
	} else {
		if (queryCount > 0) {
			sellPrice = POSITIVE_INFINITY;
			buyPrice = calculateBuyPrice(articleId, availableQuant, queryCount);
		} else {
			sellPrice = 0.0;
			buyPrice = 0.0;
		}
	}
	articleQuery->unlock();
}

float Market::calculateBuyPrice(unsigned articleId, unsigned availableQuant,
		unsigned queryCount) {
	float recentBuyPrice = queryCount / (availableQuant + 1.0);
	float historyBuyPrice = articleBuyPricesHistory.at(articleId);
	return (recentBuyPrice + historyBuyPrice) / 2.0;
}

float Market::calculateSellPrice(unsigned articleId, unsigned availableQuant,
		unsigned queryCount) {
	float recentSellPrice = queryCount / availableQuant;
	float historySellPrice = articleSellPricesHistory.at(articleId);
	return (recentSellPrice + historySellPrice) / 2.0;
}

float Market::buyArticleFromClient(QString clientId, unsigned articleId) {
	rememberClientQuery(clientId);
	stocks.at(articleId).release();
	float soldAt = articleSellPrice(clientId, articleId);
	return soldAt;
}

bool Market::sellArticleToClient(QString clientId, unsigned articleId,
		float &cash) {
	bool sold = false;
	float sellPrice = articleSellPrice(clientId, articleId);
	if (sellPrice <= cash) {
		sold = stocks.at(articleId).tryAcquire();
		if (sold) {
			cash -= sellPrice;
		}
	}
	return sold;
}

void Market::rememberClientQuery(unsigned articleId, QString clientId) {
	ClientVisits *clientVisit = &queries.at(articleId);
	QMutex *articleQuery = &locks.at(articleId);
	articleQuery->lock();
	ClientVisits::iterator visit = std::find(clientVisit.begin(),
			clientVisit.end(), clientId);
	bool clientAskedAboutArticle = visit != clientVisit.end();
	if (!clientAskedAboutArticle) {
		clientVisit->push_back(clientId);
	}
	articleQuery->unlock();
}
