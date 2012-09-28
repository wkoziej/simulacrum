/*
 * Market.cpp
 *
 *  Created on: 16-10-2011
 *      Author: wojtas
 */

#include "Market.h"

#include <QtCore/QSemaphore>
#include <QtCore/QMutex>
#include <list>
#include <vector>
#include <algorithm>
#include "World.h"

typedef std::list<std::string> ClientVisits;
typedef std::vector<QSemaphore *> ArticleStocks;
typedef std::vector<ClientVisits> ArticleQueries;
typedef std::vector<QMutex *> ArticleLocks;
typedef std::vector<float> PriceVector;

class MarketPrivate {
public:
	ArticleStocks stocks;
	ArticleQueries queries;
	ArticleLocks locks;
	PriceVector articleSellPricesHistory;
	PriceVector articleBuyPricesHistory;
};

Market::Market() {
	prv = new MarketPrivate();
	for (unsigned i = 0; i < World::NO_OF_ARTICLES; i++) {
		prv->stocks.push_back(new QSemaphore(0));
		prv->locks.push_back(new QMutex());
	}
	prv->queries.assign(World::NO_OF_ARTICLES, ClientVisits());
	prv->articleSellPricesHistory.assign(World::NO_OF_ARTICLES, 0);
	prv->articleBuyPricesHistory.assign(World::NO_OF_ARTICLES, 0);
}

Market::~Market() {
	delete prv;
}

bool Market::isArticleAvailable(std::string clientId, unsigned articleId,
		float articleQuant) {
	rememberClientQuery(articleId, clientId);
	return prv->stocks.at(articleId)->available() > articleQuant;
}

unsigned Market::queriesCount (unsigned articleId) {
	return prv->queries.at(articleId).size();
}

float Market::articleSellPrice(std::string clientId, unsigned articleId) {
	float sellPrice = 0.0;
	float buyPrice = 0.0;
	// Zapisz zainteresowanie klienta -> wpływ na cenę sprzedaży
	rememberClientQuery(articleId, clientId);
	getPrices(articleId, sellPrice, buyPrice);
	return sellPrice;
}

float Market::articleBuyPrice(unsigned articleId) {
	float sellPrice = 0.0;
	float buyPrice = 0.0;
	getPrices(articleId, sellPrice, buyPrice);
	return buyPrice;
}

float Market::articleSellPrice(unsigned articleId) {
	float sellPrice = 0.0;
	float buyPrice = 0.0;
	getPrices(articleId, sellPrice, buyPrice);
	return sellPrice;
}

unsigned Market::articleStock(unsigned articleId) {
	return prv->stocks.at(articleId)->available();
}

void Market::getPrices(unsigned articleId,
		float &sellPrice, float &buyPrice) {
	int availableQuant = prv->stocks.at(articleId)->available();
	QMutex *articleQuery = prv->locks.at(articleId);
	articleQuery->lock();
	int queryCount = prv->queries.at(articleId).size();
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
	float recentBuyPrice = (float) queryCount / (availableQuant + 1.0);
	float historyBuyPrice = prv->articleBuyPricesHistory.at(articleId);
	return (recentBuyPrice + historyBuyPrice) / 2.0;
}

float Market::calculateSellPrice(unsigned articleId, unsigned availableQuant,
		unsigned queryCount) {
	float recentSellPrice = (float) queryCount / (float) availableQuant;
	float historySellPrice = prv->articleSellPricesHistory.at(articleId);
	return (recentSellPrice + historySellPrice) / 2.0;
}

float Market::buyArticleFromClient(std::string clientId, unsigned articleId) {
	prv->stocks.at(articleId)->release();
	float soldAt = articleBuyPrice(articleId);
	return soldAt;
}

bool Market::sellArticleToClient(std::string clientId, unsigned articleId,
		float &cash) {
	bool sold = false;
	rememberClientQuery(articleId, clientId);
	float sellPrice = articleSellPrice(clientId, articleId);
	if (sellPrice <= cash) {
		sold = prv->stocks.at(articleId)->tryAcquire();
		if (sold) {
			cash -= sellPrice;
		}
	}
	return sold;
}

void Market::rememberClientQuery(unsigned articleId, std::string clientId) {
	ClientVisits *clientVisit = &prv->queries.at(articleId);
	QMutex *articleQuery = prv->locks.at(articleId);
	articleQuery->lock();
	ClientVisits::iterator visit = find(clientVisit->begin(),
			clientVisit->end(), clientId);
	bool clientAskedAboutArticle = visit != clientVisit->end();
	if (!clientAskedAboutArticle) {
		clientVisit->push_back(clientId);
	}
	articleQuery->unlock();
}
