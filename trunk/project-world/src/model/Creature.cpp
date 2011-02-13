/*
 * Creature.cpp
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#include "Creature.h"
#include "Config.h"
#include <numeric>

LoggerPtr Creature::logger(Logger::getLogger("creature"));

float NEEDS_BOUNDS[3] = { 0.1, 5.0, 0.1 };

float TALENTS_V_BOUNDS[3] = { 0.1, 10.0, 0.1 };

float TALENTS_F_BOUNDS[3] = { 0.1, 5.0, 0.1 };

float VELOCITY_BOUNDS[3] = { 0.0, 10.0, 0.1 };

float DIRECTION_BOUNDS[3] = { 0.0, 3.0, 1.0 };

float Objective(GAGenome&g) {
	Creature *c = (Creature *) &g;
	CreatureFenotype * f = (CreatureFenotype *) c->evalData();
	float sum = std::accumulate(f->missedProductQuants.begin(), f->missedProductQuants.end(), 0.0);
	LOG4CXX_DEBUG(Creature::getLogger(), "Objective -> sum : " << sum);
	return sum > 0.0 ? 0.0 : 0.5;
}

GAEvalData* CreatureFenotype::clone() const {
	CreatureFenotype *cf = new CreatureFenotype();
	cf->copy(*this);
	return cf;
}

void CreatureFenotype::copy(const GAEvalData&src) {
	missedProductQuants = (((CreatureFenotype &) src).missedProductQuants);
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

void Creature::RandomInitializer(GAGenome &g) {
	GARealGenome *h = (GARealGenome *) &g;
	int i = 0;
	unsigned j = 0;
	for (; i < NO_OF_RESOURCES; i++) {
		// Potrzeby
		h->gene(j++, randBetweenAndStepped(NEEDS_BOUNDS[0], NEEDS_BOUNDS[1],
				NEEDS_BOUNDS[2]));
		// Talenty (prędkość przetwarzania) Velocity
		h->gene(j++, randBetweenAndStepped(TALENTS_V_BOUNDS[0],
				TALENTS_V_BOUNDS[1], TALENTS_V_BOUNDS[2]));
		// Talenty (współczynnik przetwarzania) Rate
		h->gene(j++, randBetweenAndStepped(TALENTS_F_BOUNDS[0],
				TALENTS_F_BOUNDS[1], TALENTS_F_BOUNDS[2]));
	}
	// Prędkość poruszania się
	h->gene(j++, randBetweenAndStepped(VELOCITY_BOUNDS[0], VELOCITY_BOUNDS[1],
			VELOCITY_BOUNDS[2]));
	// Droga do przebycia
	i = 0;
	for (; i < MAX_POINT_ON_ROAD; i++) {
		// 4 kierunki
		h->gene(j++, randBetweenAndStepped(DIRECTION_BOUNDS[0],
				DIRECTION_BOUNDS[1], DIRECTION_BOUNDS[2]));
	}
}

GARealAlleleSetArray Creature::alleles;

GARealAlleleSetArray Creature::allelesDefinition() {
	if (alleles.size() == 0) {

		int i = 0;
		for (; i < NO_OF_RESOURCES; i++) {
			// Potrzeby
			alleles.add(NEEDS_BOUNDS[0], NEEDS_BOUNDS[1], NEEDS_BOUNDS[2]);
			// Talenty (prędkość przetwarzania) Velocity
			alleles.add(TALENTS_V_BOUNDS[0], TALENTS_V_BOUNDS[1],
					TALENTS_V_BOUNDS[2]);
			// Talenty (współczynnik przetwarzania) Rate
			alleles.add(TALENTS_F_BOUNDS[0], TALENTS_F_BOUNDS[1],
					TALENTS_F_BOUNDS[2]);
		}
		// Prędkość poruszania się
		alleles.add(VELOCITY_BOUNDS[0], VELOCITY_BOUNDS[1], VELOCITY_BOUNDS[2]);
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

float Creature::produce(float resourceQuantity, float &resourceUsed,
		unsigned index) {
	LOG4CXX_TRACE(logger, "produce");
	float processingRate = getProcessingRate(index);
	LOG4CXX_DEBUG(logger, "processingRate " << this << processingRate);
	float processingVelocity = getProcessingVelocity(index);
	LOG4CXX_DEBUG(logger, "processingVelocity " << this << processingVelocity);
	resourceUsed = resourceQuantity >= processingVelocity ? processingVelocity
			: resourceQuantity;
	float productQuant = resourceUsed * processingRate;
	LOG4CXX_DEBUG(logger, "resourceQuantity: " << resourceQuantity << ", processingRate: " << processingRate << ", processingVelocity: " << processingVelocity << ", resourceUsed: " << resourceUsed);
	LOG4CXX_DEBUG(logger, "creature " << this << " produced " << productQuant);
	return productQuant;
}

void Creature::feed(float productAmount, float &productEaten,
		bool &amountSuffice, unsigned productIndex) {
	LOG4CXX_TRACE(logger, "feed");
	float needs = getNeedAmount(productIndex);
	if ((amountSuffice = (productAmount >= needs))) {
		productEaten = needs;
		productMissed(0, productIndex);
	} else { // productAmount < needs
		productMissed(needs - productAmount, productIndex);
		productEaten = productAmount;
	}
	LOG4CXX_DEBUG(logger, "productIndex:" << productIndex << ", productAmount: " <<productAmount<< " needs: " << needs);
}

void Creature::productMissed(float quant, unsigned productIndex) {
	CreatureFenotype * evalData = (CreatureFenotype *) this->evd;
	evalData->missedProductQuants.at(productIndex) = quant;
}

float Creature::getVelocity() {
	//GARealGenome *g = (GARealGenome *) genome;
	return this->gene(3 * NO_OF_RESOURCES);
}

Creature::Directions Creature::nextDirection(unsigned step) {
	Creature::Directions direction = Creature::NoDirection;
	if (step < MAX_POINT_ON_ROAD) {
		float d = this->gene(3 * NO_OF_RESOURCES + 1);
		LOG4CXX_DEBUG(logger, " nextDirection (" << step << ") = " << d);
		if (d == 0.0) {
			direction = Creature::DirLeft;
		} else if (d == 1.0) {
			direction = Creature::DirRight;
		} else if (d == 2.0) {
			direction = Creature::DirUp;
		} else {
			direction = Creature::DirDown;
		}
	}
	LOG4CXX_DEBUG(logger, " direction (" << step << ") = " << direction);
	return direction;
}

float Creature::getNeedAmount(unsigned productIndex) {
	LOG4CXX_TRACE(logger, "getNeedAmount [" << productIndex << "]");
	LOG4CXX_DEBUG(logger, "this " << this);
	return this->gene(productIndex * 3);
}

float Creature::getProcessingVelocity(unsigned index) {
	return this->gene(index * 3 + 1);
}

float Creature::getProcessingRate(unsigned index) {
	LOG4CXX_DEBUG(logger, "getProcessingRate [" << index << "]");
	LOG4CXX_DEBUG(logger, "this " << this);
	return this->gene(index * 3 + 2);
}

Creature::~Creature() {
	delete this->evd;
}
