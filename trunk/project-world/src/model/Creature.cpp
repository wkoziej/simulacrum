/*
 * Creature.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#include "Creature.h"
#include "Config.h"
#include "World.h"
#include <numeric>

LoggerPtr Creature::logger(Logger::getLogger("creature"));

float NEEDS_BOUNDS[3] = { 0.05, 1.0, 0.05 };
float TALENTS_F_BOUNDS[3] = { 0.05, 1.0, 0.05 };
float DIRECTION_BOUNDS[3] = { 0.0, 3.0, 1.0 };

#define TALENT_BITS_COUNT 3
#define NEED_BITS_COUNT 2
#define CREATING_PRODUCT_BIT 1
#define PROCESSING_RATIO_BIT 2
#define NEED_RATIO_BIT 1

float Objective(GAGenome &g) {
	Creature *c = (Creature *) &g;
	CreatureFenotype * fenotype = c->getFenotype();
	World *world = World::getWorld();
	Field *field = world->fields.at(fenotype->fieldCoordX).at(
			fenotype->fieldCoordY);
	/*Population *population = world->populations.at(fenotype->fieldCoordX).at(
	 fenotype->fieldCoordY);*/
	// Możliwe funkcje celu
	// 1. Stosunek wartości wytworzonych produktów do wartości zużytych surowców jest największy
	// 2. Wartość wytworzonych produktów jest największa


	// wersja 1
	// Wartość wytworzonych produktów
	float productsValueSum = 0.0;
	for (int i = 0; i < World::NO_OF_PRODUCTS; i++) {
		productsValueSum += field->productPrice(i)
				* fenotype->createdProductQuants.at(i);
	}
	// Wartość zużytych surowców
	float resourcesValueSum = 0.0;
	for (int i = 0; i < World::NO_OF_PRODUCTS; i++) {
		resourcesValueSum += field->resourcePrice(i)
				* fenotype->usedResourcesQuants.at(i);
	}
	// Stosunek wartości produktu do wartości zużycia
	float value = POSITIVE_INFINITY;
	if (resourcesValueSum > 0)
		value = productsValueSum / resourcesValueSum;

	fenotype->objectiveValue = value;
	LOG4CXX_DEBUG(Creature::getLogger(), "Objective  : " << value);
	return value;
}

CreatureFenotype::CreatureFenotype(const Population * population) {
	createdProductQuants.assign(World::NO_OF_PRODUCTS, 0);
	usedResourcesQuants.assign(World::NO_OF_RESOURCES, 0);
	//notSatisfiedNeedsQuants.assign(NO_OF_PRODUCTS, 0);
	yearsOld = 0;
	previousFieldIdexes = std::pair<unsigned, unsigned>(0, 0);
	fieldCoordX = -1;
	fieldCoordY = -1;
	objectiveValue = 0;
	this->population = population;
}
;

GAEvalData* CreatureFenotype::clone() const {
	CreatureFenotype *cf = new CreatureFenotype();
	cf->copy(*this);
	return cf;
}

void CreatureFenotype::copy(const GAEvalData&src) {
	createdProductQuants = (((CreatureFenotype &) src).createdProductQuants);
	usedResourcesQuants = (((CreatureFenotype &) src).usedResourcesQuants);
	//	notSatisfiedNeedsQuants
	//			= (((CreatureFenotype &) src).notSatisfiedNeedsQuants);
	objectiveValue = (((CreatureFenotype &) src).objectiveValue);
	yearsOld = (((CreatureFenotype &) src).yearsOld);
	fieldCoordX = (((CreatureFenotype &) src).fieldCoordX);
	fieldCoordY = (((CreatureFenotype &) src).fieldCoordY);
	population = (((CreatureFenotype &) src).population);
}

GARealAlleleSetArray Creature::alleles;

int Creature::noOfTalents() {
	return getFenotype()->population->getCreaturesTalentsCount();
}

int Creature::noOfNeeds() {
	return getFenotype()->population->getCreaturesNeedsCount();
}

GARealAlleleSetArray Creature::allelesDefinition() {
	if (alleles.size() == 0) {
		int i = 0;
		for (; i < Creature::noOfTalents(); i++) {
			// Indeks zasobu
			alleles.add(0, World::NO_OF_RESOURCES - 1, 1);
			// Indeks produktu
			alleles.add(0, World::NO_OF_PRODUCTS - 1, 1);
			// Talenty (współczynnik przetwarzania) Rate
			alleles.add(TALENTS_F_BOUNDS[0], TALENTS_F_BOUNDS[1],
					TALENTS_F_BOUNDS[2]);
		}
		for (i = 0; i < Creature::noOfNeeds(); i++) {
			// Indeks produktu
			alleles.add(0, World::NO_OF_PRODUCTS - 1, 1);
			// Potrzeba
			alleles.add(NEEDS_BOUNDS[0], NEEDS_BOUNDS[1], NEEDS_BOUNDS[2]);
		}
		// Droga do przebycia
		i = 0;
		for (; i < MAX_POINT_ON_ROAD; i++) {
			// 4 kierunki
			alleles.add(DIRECTION_BOUNDS[0], DIRECTION_BOUNDS[1],
					DIRECTION_BOUNDS[2]);
		}
	}
	return alleles;
}

void Creature::RandomInitializer(GAGenome &g) {
	GARealGenome *h = (GARealGenome *) &g;
	int i = 0;
	unsigned j = 0;
	for (; i < species->noOfTalents(); i++) {
		// Indeks zasobu
		h->gene(j++, randBetweenAndStepped(0, World::NO_OF_RESOURCES - 1, 1));
		// Indeks produktu
		h->gene(j++, randBetweenAndStepped(0, World::NO_OF_PRODUCTS - 1, 1));
		// Wspolczynnik przetwarzania
		h->gene(j++, randBetweenAndStepped(TALENTS_F_BOUNDS[0],
				TALENTS_F_BOUNDS[1], TALENTS_F_BOUNDS[2]));
	}
	// Potrzeby
	for (i = 0; i < Creature::noOfNeeds(); i++) {
		// Indeks produktu
		h->gene(j++, randBetweenAndStepped(0, World::NO_OF_PRODUCTS - 1, 1));
		// Wspolczynnik przetwarzania
		h->gene(j++, randBetweenAndStepped(NEEDS_BOUNDS[0], NEEDS_BOUNDS[1],
				NEEDS_BOUNDS[2]));
	}
	// Droga do przebycia
	i = 0;
	for (; i < MAX_POINT_ON_ROAD; i++) {
		// 4 kierunki
		h->gene(j++, randBetweenAndStepped(DIRECTION_BOUNDS[0],
				DIRECTION_BOUNDS[1], DIRECTION_BOUNDS[2]));
	}
}

Creature::Creature(const Population *population) :
	GARealGenome(Creature::allelesDefinition(), Objective) {
	LOG4CXX_TRACE(logger, "Creature::Creature");
	evalData(CreatureFenotype(population));
	initializer(RandomInitializer);
	initialize();
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

void Creature::JSONInitializer(GAGenome &g) {
	GARealGenome *h = (GARealGenome *) &g;
	JSONObject *creature = (JSONObject *) h->userData();
	JSONArray talents = creature->at(L"talents")->AsArray();
	int talentsSize = talents.size();
	LOG4CXX_DEBUG(logger, "Talents size: " << talentsSize << ", NO_OF_TALENTS: " << Creature::noOfTalents());
	int i;
	int geneShift = 0;
	for (i = 0; i < talentsSize; i++) {
		JSONArray talent = talents.at(i)->AsArray();
		// surowiec
		h->gene(geneShift++, talent.at(0)->AsNumber());
		// Indeks produktu
		h->gene(geneShift++, talent.at(1)->AsNumber());
		// Wspolczynnik przetwarzania
		h->gene(geneShift++, talent.at(2)->AsNumber());
	}
	LOG4CXX_DEBUG(logger, "Needs size: " << talentsSize << ", NO_OF_NEEDS: " << Creature::noOfNeeds());
	JSONArray needs = creature->at(L"needs")->AsArray();
	int needsSize = needs.size();
	for (i = 0; i < needsSize; i++) {
		JSONArray need = needs.at(i)->AsArray();
		// Produkt
		h->gene(geneShift++, need.at(0)->AsNumber());
		// Potrzeba
		h->gene(geneShift++, need.at(1)->AsNumber());
	}
	JSONArray directions = creature->at(L"directions")->AsArray();
	int directionsSize = directions.size();
	for (i = 0; i < directionsSize; i++) {
		// Kierunek
		h->gene(geneShift++, directions.at(i)->AsNumber());
	}
}

Creature::Creature(const Population *population, JSONObject &creature) :
	GARealGenome(Creature::allelesDefinition(), Objective) {
	// Wypełnij tymczasową strukturę z definicją osobnika
	userData(&creature);
	evalData(CreatureFenotype(population));
	initializer(JSONInitializer);
	initialize();
	// Można już wyczyścić dane
	userData(NULL);
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

float Creature::randBetweenAndStepped(float min, float max, float step) {
	float zeroOne = (random() / (float) RAND_MAX);
	float randBetween = min + zeroOne * (max - min);
	int intVal = randBetween / step;
	return intVal * step;
}

float Creature::produce(float resourceQuantity, unsigned index) {
	LOG4CXX_TRACE(logger, "produce");
	float productQuant = 0.0;
	if (resourceQuantity > 0.0) {
		int productIndex = -1;
		float processingRate = getProcessingRateAndProductIndex(index,
				productIndex);
		if (processingRate > 0.0) {
			LOG4CXX_DEBUG(logger, "processingRate " << this << processingRate);
			productQuant = resourceQuantity * processingRate
					* getFenotype()->performanceRatio;
			LOG4CXX_DEBUG(logger, "resourceQuantity: " << resourceQuantity << ", processingRate: " << processingRate );
			LOG4CXX_DEBUG(logger, "creature " << this << " produced " << productQuant);
			getFenotype()->createdProductQuants.at(productIndex) = productQuant;
		}
	}
	return productQuant;
}

void Creature::feed(float productAmount, unsigned productIndex) {
	LOG4CXX_TRACE(logger, "feed");
	float needRatio = getNeedOfProductRatio(productIndex);
	if (productAmount > 1.0)
		productAmount = 1.0;
	//float notSatisfiedNeed = needRatio - productAmount;
	// getFenotype()->notSatisfiedNeedsQuants.at(productIndex) = notSatisfiedNeed <= 0 ? 0 : notSatisfiedNeed;
	getFenotype()->performanceRatio += productAmount * needRatio;
	LOG4CXX_DEBUG(logger, "productIndex:" << productIndex << ", productAmount: " << productAmount << " needs: " << needRatio);
}

Creature::Directions Creature::nextDirection(unsigned step) {
	Creature::Directions direction = Creature::NoDirection;
	if (step < MAX_POINT_ON_ROAD) {
		float direction =
				this->gene(TALENT_BITS_COUNT * noOfTalents() + NEED_BITS_COUNT * noOfNeeds() + step);
		LOG4CXX_DEBUG(logger, " nextDirection (" << step << ") = " << direction);
		if (direction == 0.0) {
			direction = Creature::DirLeft;
		} else if (direction == 1.0) {
			direction = Creature::DirRight;
		} else if (direction == 2.0) {
			direction = Creature::DirUp;
		} else {
			direction = Creature::DirDown;
		}
	}
	LOG4CXX_DEBUG(logger, " direction (" << step << ") = " << direction);
	return direction;
}

float Creature::getNeedOfProductRatio(unsigned productIndex) {
	float needAmount = 0.0;
	float summary = 0.0;
	// Znajdź gen odpowiadający za potrzebę danego produktu
	int genIndex = -1;
	for (int i = 0; i < noOfNeeds(); i++) {
		int index = noOfTalents() * TALENT_BITS_COUNT + NEED_BITS_COUNT * i;
		if (gene(index) == productIndex) {
			genIndex = index;
		}
		summary += gene(index + NEED_RATIO_BIT);
	}
	if (genIndex != -1) {
		needAmount = gene(genIndex + NEED_RATIO_BIT) / summary;
	}
	LOG4CXX_DEBUG(logger, "getNeedOfProductRatio: productIndex  = " << productIndex << ", needAmount = " << needAmount);
	return needAmount;
}

float Creature::getNeedOfResource(unsigned resourceIndex) {
	int productIndex;
	return getProcessingRateAndProductIndex(resourceIndex, productIndex);
}

float Creature::getProcessingRateAndProductIndex(unsigned resourceIndex,
		int &productIndex) {
	float rate = 0.0;
	float summary = 0.0;
	productIndex = -1;
	// Znajdź gen odpowiadający za potrzebę danego produktu
	int genIndex = -1;
	for (int i = 0; i < noOfTalents(); i++) {
		int index = TALENT_BITS_COUNT * i;
		if (gene(index) == resourceIndex) {
			genIndex = index;
		}
		summary += gene(index + PROCESSING_RATIO_BIT);
	}
	if (genIndex != -1) {
		productIndex = gene(genIndex + CREATING_PRODUCT_BIT);
		rate = gene(genIndex + PROCESSING_RATIO_BIT) / summary;
	}
	LOG4CXX_DEBUG(logger, "getProcessingRateAndProductIndex: resourceIndex = " << resourceIndex << ", productIndex = " << productIndex << ", procesingRate " << rate);
	return rate;
}

Creature::~Creature() {
	delete this->evd;
}
