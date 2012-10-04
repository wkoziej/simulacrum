/*
 * Market.h
 *
 *  Created on: 16-10-2011
 *      Author: wojtas
 */

#ifndef MARKET_H_
#define MARKET_H_
#include <string>
class MarketPrivate;

class Market {
public:
	Market();
	virtual ~Market();
	bool isArticleAvailable(std::string clientId, unsigned articleId,
			float articleQuant);
	unsigned queriesCount (unsigned articleId);
	float articleSellPrice(std::string clientId, unsigned articleId);
	float articleBuyPrice(unsigned articleId);
	// Nie wplywa na cenę, bo nie pyta się klient
	float articleSellPrice(unsigned articleId);
	unsigned articleStock(unsigned articleId);
	float buyArticleFromClient(std::string clientId, unsigned articleId);
	bool sellArticleToClient(std::string clientId, unsigned articleId,
			float &cash);
private:
	MarketPrivate *prv;

	void getPrices(unsigned articleId, float &sellPrice, float &buyPrice);
	float calculateBuyPrice(unsigned articleId, unsigned availableQuant,
			unsigned queryCount);
	float calculateSellPrice(unsigned articleId, unsigned availableQuant,
			unsigned queryCount);

	//ClientVisits marketClients;
	// articleHits;
	void rememberClientQuery(unsigned articleId, std::string clientId);

};

#endif /* MARKET_H_ */
