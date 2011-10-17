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
	float articleSellPrice(std::string clientId, unsigned articleId);
	float articleBuyPrice(std::string clientId, unsigned articleId);
	float buyArticleFromClient(std::string clientId, unsigned articleId);
	bool sellArticleToClient(std::string clientId, unsigned articleId,
			float &cash);
private:
	MarketPrivate *prv;

	void getPrices(std::string clientId, unsigned articleId,
			float &sellPrice, float &buyPrice);
	float calculateBuyPrice(unsigned articleId, unsigned availableQuant,
			unsigned queryCount);
	float calculateSellPrice(unsigned articleId, unsigned availableQuant,
			unsigned queryCount);

	//ClientVisits marketClients;
	// articleHits;
	void rememberClientQuery(unsigned articleId, std::string clientId);

};

#endif /* MARKET_H_ */
