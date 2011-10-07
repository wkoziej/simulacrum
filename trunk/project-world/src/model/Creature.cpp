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

float Objective(GAGenome &g) {
	Creature *c = (Creature *) &g;
	CreatureFenotype * fenotype = c->getFenotype();
	World *world = World::getWorld();
	Field *field = world->fields.at(fenotype->fieldCoordX).at(
			fenotype->fieldCoordY);
	Population *population = world->populations.at(fenotype->fieldCoordX).at(
			fenotype->fieldCoordY);
	// Możliwe funkcje celu
	// 1. Stosunek wartości wytworzonych produktów do wartości zużytych surowców jest największy
	// 2. Wartość wytworzonych produktów jest największa


	// wersja 1
	// Wartość wytworzonych produktów
	float productsValueSum = 0.0;
	for (int i = 0; i < NO_OF_PRODUCTS; i++) {
		productsValueSum += population->productPrice(i)
				* fenotype->createdProductQuants.at(i);
	}
	// Wartość zużytych surowców
	float resourcesValueSum = 0.0;
	for (int i = 0; i < NO_OF_PRODUCTS; i++) {
		resourcesValueSum += population->resourcePrice(i)
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

CreatureFenotype::CreatureFenotype() {
	createdProductQuants.assign(NO_OF_PRODUCTS, 0);
	usedResourcesQuants.assign(NO_OF_RESOURCES, 0);
	notSatisfiedNeedsQuants.assign(NO_OF_PRODUCTS, 0);
	yearsOld = 0;
	previousFieldIdexes = std::pair<unsigned, unsigned>(0, 0);
	fieldCoordX = -1;
	fieldCoordY = -1;
	objectiveValue = 0;
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
	notSatisfiedNeedsQuants
			= (((CreatureFenotype &) src).notSatisfiedNeedsQuants);
	objectiveValue = (((CreatureFenotype &) src).objectiveValue);
	yearsOld = (((CreatureFenotype &) src).yearsOld);
	fieldCoordX = (((CreatureFenotype &) src).fieldCoordX);
	fieldCoordY = (((CreatureFenotype &) src).fieldCoordY);
}

GARealAlleleSetArray Creature::alleles;

int Creature::noOfTalents() {
	return 2;
}

int Creature::noOfNeeds() {
	return 3;
}

GARealAlleleSetArray Creature::allelesDefinition() {
	if (alleles.size() == 0) {

		int i = 0;
		for (; i < Creature::noOfTalents(); i++) {
			// Indeks zasobu
			alleles.add(0, NO_OF_RESOURCES - 1, 1);
			// Indeks produktu
			alleles.add(0, NO_OF_PRODUCTS - 1, 1);
			// Talenty (współczynnik przetwarzania) Rate
			alleles.add(TALENTS_F_BOUNDS[0], TALENTS_F_BOUNDS[1],
					TALENTS_F_BOUNDS[2]);
		}

		for (i = 0; i < Creature::noOfNeeds(); i++) {
			// Indeks produktu
			alleles.add(0, NO_OF_PRODUCTS - 1, 1);
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
	for (; i < Creature::noOfTalents(); i++) {
		// Indeks zasobu
		h->gene(j++, randBetweenAndStepped(0, NO_OF_RESOURCES - 1, 1));
		// Indeks produktu
		h->gene(j++, randBetweenAndStepped(0, NO_OF_PRODUCTS - 1, 1));
		// Wspolczynnik przetwarzania
		h->gene(j++, randBetweenAndStepped(TALENTS_F_BOUNDS[0],
				TALENTS_F_BOUNDS[1], TALENTS_F_BOUNDS[2]));
	}
	// Potrzeby
	for (i = 0; i < Creature::noOfNeeds(); i++) {
		// Indeks produktu
		h->gene(j++, randBetweenAndStepped(0, NO_OF_PRODUCTS - 1, 1));
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

Creature::Creature() :
	GARealGenome(Creature::allelesDefinition(), Objective) {
	LOG4CXX_TRACE(logger, "Creature::Creature");
	initializer(RandomInitializer);
	initialize();
	this->evd = new CreatureFenotype();
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
			productQuant = resourceQuantity * processingRate;
			LOG4CXX_DEBUG(logger, "resourceQuantity: " << resourceQuantity << ", processingRate: " << processingRate );
			LOG4CXX_DEBUG(logger, "creature " << this << " produced " << productQuant);
			getFenotype()->createdProductQuants.at(productIndex) = productQuant;
		}
	}
	return productQuant;
}

void Creature::feed(float productAmount, unsigned productIndex) {
	LOG4CXX_TRACE(logger, "feed");
	float need = getNeedOfProduct(productIndex);
	float notSatisfiedNeed = need - productAmount;
	getFenotype()->notSatisfiedNeedsQuants.at(productIndex) = notSatisfiedNeed
			<= 0 ? 0 : notSatisfiedNeed;
	LOG4CXX_DEBUG(logger, "productIndex:" << productIndex << ", productAmount: " <<productAmount<< " needs: " << need);
}

Creature::Directions Creature::nextDirection(unsigned step) {
	Creature::Directions direction = Creature::NoDirection;
	if (step < MAX_POINT_ON_ROAD) {
		float direction =
				this->gene(3 * noOfTalents() + 2 * noOfNeeds() + step);
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

float Creature::getNeedOfProduct(unsigned productIndex) {
	LOG4CXX_TRACE(logger, "getNeedAmount [" << productIndex << "]");
	LOG4CXX_DEBUG(logger, "this " << this);
	float needAmount = 0.0;
	// Znajdź gen odpowiadający za potrzebę danego produktu
	int genIndex = -1;
	for (int i = 0; i < noOfNeeds(); i++) {
		int index = noOfTalents() * 3 + 2 * i;
		if (gene(index) == productIndex) {
			genIndex = index;
			break;
		}
	}
	if (genIndex != -1) {
		needAmount = gene(genIndex + 1);
	}
	return needAmount;
}

float Creature::getNeedOfResource(unsigned resourceIndex) {
	int productIndex;
	return getProcessingRateAndProductIndex(resourceIndex, productIndex);
}

float Creature::getProcessingRateAndProductIndex(unsigned resourceIndex,
		int &productIndex) {
	LOG4CXX_DEBUG(logger, "getProcessingRate [" << resourceIndex << "]");
	LOG4CXX_DEBUG(logger, "this " << this);
	float rate = 0.0;
	// Znajdź gen odpowiadający za potrzebę danego produktu
	int genIndex = -1;
	for (int i = 0; i < noOfTalents(); i++) {
		int index = 3 * i;
		if (gene(index) == resourceIndex) {
			genIndex = index;
			break;
		}
	}
	if (genIndex != -1) {
		productIndex = gene(genIndex + 1);
		rate = gene(genIndex + 2);
	}
	return rate;
}

Creature::~Creature() {
	delete this->evd;
}
