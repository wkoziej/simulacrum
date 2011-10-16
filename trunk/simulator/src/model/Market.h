/*
 * Market.h
 *
 *  Created on: 16-10-2011
 *      Author: wojtas
 */

#include <QtCore/QSemaphore>
#include <QtCore/QMutex>
#include <list>

#ifndef MARKET_H_
#define MARKET_H_

typedef std::list<QString> ClientVisits;
typedef std::vector<QSemaphore > ArticleStocks;
typedef std::vector<ClientVisits> ArticleQueries;
typedef std::vector<QMutex> ArticleLocks;
typedef std::vector<float> FloatVector;

class Market {
public:
	Market();
	virtual ~Market();
	bool isArticleAvailable(QString clientId, unsigned articleId,
			float articleQuant);
	float articleSellPrice(QString clientId, unsigned articleId);
	float articleBuyPrice(QString clientId, unsigned articleId);
	float buyArticleFromClient(QString clientId, unsigned articleId);
	bool sellArticleToClient(QString clientId, unsigned articleId,
			float &cash);
private:

	void getPrices(QString clientId, unsigned articleId,
			float &sellPrice, float &buyPrice);
	float calculateBuyPrice(unsigned articleId, unsigned availableQuant,
			unsigned queryCount);
	float calculateSellPrice(unsigned articleId, unsigned availableQuant,
			unsigned queryCount);

	ArticleStocks stocks;
	ArticleQueries queries;
	ArticleLocks locks;
	FloatVector articleSellPricesHistory;
	FloatVector articleBuyPricesHistory;
	//ClientVisits marketClients;
	// articleHits;
	void rememberClientQuery(unsigned articleId, QString clientId);

};

#endif /* MARKET_H_ */
