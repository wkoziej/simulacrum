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

// Liczba róznych strategii wyboru aktywności
// przykłady :
// 1. Czynności o rosnącej liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 2. Czynności o malejącej  liczbie argumentów (0 argumentowe, 1 argumentowe, ...)
// 3. Na zmianę zaczynając od czynności od najmniejszej liczby argumentów
#define CHOOSING_ACTIVITY_STRATEGY_COUNT 1

//float NEEDS_BOUNDS[3] = { 0.05, 1.0, 0.05 };
//float TALENTS_F_BOUNDS[3] = { 0.05, 1.0, 0.05 };
//float DIRECTION_BOUNDS[3] = { 0.0, 3.0, 1.0 };

//#define TALENT_BITS_COUNT 3
//#define NEED_BITS_COUNT 2
//#define CREATING_PRODUCT_BIT 1
//#define PROCESSING_RATIO_BIT 2
//#define NEED_RATIO_BIT 1

float Objective(GAGenome &g) {
	Creature *c = (Creature *) &g;
	CreatureFenotype * fenotype = c->getFenotype();
	float value = 0.0;
	//World *world = World::getWorld();
	const Field *field = fenotype->field;
	// Możliwe funkcje celu
	// 1. Stosunek wartości wytworzonych produktów do wartości zużytych surowców jest największy
	// 2. Wartość wytworzonych produktów jest największa


	// wersja 1
	// Wartość wytworzonych produktów
	/*float productsValueSum = 0.0;
	 for (int i = 0; i < World::NO_OF_PRODUCTS; i++) {
	 productsValueSum += field->productPrice(i)
	 * fenotype->createdProductQuants.at(i);
	 }
	 if (productsValueSum != 0) {
	 // Wartość zużytych surowców
	 float resourcesValueSum = 0.0;
	 for (int i = 0; i < World::NO_OF_PRODUCTS; i++) {
	 resourcesValueSum += field->resourcePrice(i)
	 * fenotype->usedResourcesQuants.at(i);
	 }
	 // Stosunek wartości produktu do wartości zużycia
	 value = POSITIVE_INFINITY;
	 if (resourcesValueSum > 0)
	 value = productsValueSum / resourcesValueSum;
	 }*/
	LOG4CXX_DEBUG(Creature::getLogger(), "Objective NOT IMPLEMENTED !!!!  : " << value);
	return value;
}

LoggerPtr CreatureFenotype::logger(Logger::getLogger("creatureFenotype"));

CreatureFenotype::CreatureFenotype(const CreaturesPopulation * population,
		const Field *field) {
	gainedArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	lostArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	articleStocks.assign(World::NO_OF_ARTICLES, 0);
	yearsOld = 0;
	previousFieldIdexes = std::pair<unsigned, unsigned>(0, 0);
	this->population = population;
	this->field = field;
}
;

GAEvalData* CreatureFenotype::clone() const {
	CreatureFenotype *cf = new CreatureFenotype(this->population, this->field);
	cf->copy(*this);
	return cf;
}

void CreatureFenotype::copy(const GAEvalData&src) {
	LOG4CXX_TRACE(logger, "CreatureFenotype::copy");
	gainedArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	lostArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	lostArticlesQuants.assign(World::NO_OF_ARTICLES, 0);
	yearsOld = 0;// (((CreatureFenotype &) src).yearsOld);
	population = (((CreatureFenotype &) src).population);
	field = (((CreatureFenotype &) src).field);
	wallet = 0;
}

GARealAlleleSetArray Creature::alleles;

/*int Creature::noOfTalents() {
 return getFenotype()->population->getCreaturesTalentsCount();
 }

 int Creature::noOfNeeds() {
 return getFenotype()->population->getCreaturesNeedsCount();
 }*/

std::string Creature::genomeStr() const {
	stringstream buf;
	buf << *this;
	return buf.str();
}

GARealAlleleSetArray Creature::allelesDefinition(
		const CreaturesPopulation *population) {
	if (alleles.size() == 0) {
		// Strategia
		alleles.add(0, CHOOSING_ACTIVITY_STRATEGY_COUNT - 1, 1);
		int i = 0;
		// Kodowanie czynności bezargumentowych (np. ruch w lewo)
		for (; i < population->get0ArgActivitiesRoom(); i++) {
			alleles.add(0, population->get0ArgActivitiesCount(), 1);
		}
		// Kodowanie czynności jednoargumentowych (np. kupno towaru)
		for (i = 0; i < population->get1ArgActivitiesRoom(); i++) {
			// Indeks czynności
			alleles.add(0, population->get1ArgActivitiesCount() - 1, 1);
			// Argument - artykuł
			alleles.add(0, World::NO_OF_ARTICLES - 1, 1);
		}
		// Kodowanie czynności dwuargumentowych (np. wymiana towaru za towar)
		for (i = 0; i < population->get2ArgActivitiesRoom(); i++) {
			// Indeks czynności
			alleles.add(0, population->get2ArgActivitiesCount() - 1, 1);
			// Argument 1 - artykuł
			alleles.add(0, World::NO_OF_ARTICLES - 1, 1);
			// Argument 2 - artykuł
			alleles.add(0, World::NO_OF_ARTICLES - 1, 1);
		}
	}
	return alleles;
}

/*void Creature::RandomInitializer(GAGenome &g) {
 Creature *h = (Creature*) &g;
 int i = 0;
 unsigned j = 0;
 for (; i < h->noOfTalents(); i++) {
 // Indeks zasobu
 h->gene(j++, randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1));
 // Indeks produktu
 h->gene(j++, randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1));
 // Wspolczynnik przetwarzania
 h->gene(j++, randBetweenAndStepped(TALENTS_F_BOUNDS[0],
 TALENTS_F_BOUNDS[1], TALENTS_F_BOUNDS[2]));
 }
 // Potrzeby
 for (i = 0; i < h->noOfNeeds(); i++) {
 // Indeks produktu
 h->gene(j++, randBetweenAndStepped(0, World::NO_OF_ARTICLES - 1, 1));
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
 }*/

void Creature::DoNothingInitializer(GAGenome &g) {

}

Creature::Creature(const CreaturesPopulation *population, const Field * field) :
	GARealGenome(Creature::allelesDefinition(population), Objective) {
	LOG4CXX_TRACE(logger, "Creature::Creature");
	evalData(CreatureFenotype(population, field));
	initializer( RandomInitializer);
	initialize();
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

void Creature::JSONInitializer(GAGenome &g) {
	Creature *h = (Creature *) &g;
	JSONObject *creature = (JSONObject *) h->userData();
	/*JSONArray talents = creature->at(L"talents")->AsArray();
	 int talentsSize = talents.size();
	 LOG4CXX_DEBUG(logger, "talentsSize = " << talentsSize << ", h->noOfTalents() = " << h->noOfTalents());
	 assert (talentsSize == h->noOfTalents());
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
	 JSONArray needs = creature->at(L"needs")->AsArray();
	 int needsSize = needs.size();
	 assert(needsSize == h->noOfNeeds());
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
	 }*/
}

Creature::Creature(const CreaturesPopulation *population, const Field * field,
		JSONObject &creature) :
	GARealGenome(Creature::allelesDefinition(population), Objective) {
	// Wypełnij tymczasową strukturę z definicją osobnika
	userData(&creature);
	evalData(CreatureFenotype(population, field));
	initializer(JSONInitializer);
	initialize();
	// Można już wyczyścić dane
	userData(NULL);
	initializer(DoNothingInitializer);
	LOG4CXX_DEBUG(logger, "genome : " << *this);
}

/*void Creature::step() {
 CreatureFenotype *fenotype = getFenotype();
 fenotype->createdProductQuants.assign(World::NO_OF_PRODUCTS, 0);
 fenotype->usedResourcesQuants.assign(World::NO_OF_RESOURCES, 0);
 this->_evaluated = gaFalse;
 fenotype->yearsOld++;
 //fenotype->objectiveValue = 0;
 }*/

void Creature::changePopulation(CreaturesPopulation *from,
		CreaturesPopulation *to) {
	assert (getFenotype()->population == from);
	from->remove(this);
	to->add(this);
	getFenotype()->population = to;
}

float Creature::randBetweenAndStepped(float min, float max, float step) {
	float zeroOne = (random() / (float) RAND_MAX);
	float randBetween = min + zeroOne * (max - min);
	int intVal = randBetween / step;
	return intVal * step;
}

/*float Creature::produce(float resourceQuantity, unsigned index,
 float &resourceUsed, int &productIndex) {
 LOG4CXX_TRACE(logger, "produce");
 float productQuant = 0.0;
 resourceUsed = 0.0;
 productIndex = -1;
 if (resourceQuantity > 0.0) {

 float processingRate = getProcessingRateAndProductIndex(index,
 productIndex);
 if (processingRate > 0.0) {
 resourceUsed = resourceQuantity * processingRate;
 productQuant = resourceUsed * getPerformanceRatio();
 LOG4CXX_DEBUG(logger, "resourceQuantity = " << resourceQuantity << ", processingRate = " << processingRate << ", produced = " << productQuant);
 getFenotype()->createdProductQuants.at(productIndex)
 += productQuant;
 getFenotype()->usedResourcesQuants.at(index) += resourceUsed;
 }
 }
 this->_evaluated = gaFalse;
 return productQuant;
 }*/

/*
 void Creature::feed(float productAmount, unsigned productIndex, float &eaten) {
 LOG4CXX_TRACE(logger, "feed");
 float needRatio = getNeedOfProductRatio(productIndex);
 if (productAmount > 1.0)
 productAmount = 1.0;
 eaten = productAmount * needRatio;
 getFenotype()->increasePerformanceRatio(eaten);
 LOG4CXX_DEBUG(logger, "productIndex:" << productIndex << ", productAmount: " << productAmount << " needs: " << needRatio << ", eaten: " << eaten);
 }
 */
/*

 Creature::Directions Creature::nextDirection(unsigned step) {
 Creature::Directions direction = Creature::NoDirection;
 if (step < MAX_POINT_ON_ROAD) {
 float direction = this->gene(TALENT_BITS_COUNT * noOfTalents()
 + NEED_BITS_COUNT * noOfNeeds() + step);
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
 */

Creature* Creature::clone(GAGenome::CloneMethod flag) {
	return GARealGenome::clone(flag);
}

void Creature::copy(const Creature&) {
	GARealGenome::copy(*this);
}

Creature::~Creature() {
	delete this->evd;
}

void Creature::doAllActivities() {

}

class ProduceArticle: CreatureActivity {
public:
	void make() {
		assert (arguments.size() == 1);
		unsigned articleId = arguments.at(0);
		const Recipe *recipe = field->getRecipe(articleId);
		bool recipeExists = recipe != NULL;
		if (recipeExists) {
			FloatVector ingredients = recipe->getIngredientsVector();
			bool canProduce = fenotype->articleStocksGreterOrEqual(ingredients);
			if (canProduce) {
				FloatVector::iterator s = fenotype->articleStocks.begin();
				FloatVector::iterator i = ingredients.begin();
				for (; s!= fenotype->articleStocks.end(); s++, i++) {
					*s -= *i;
				}
			}
		}
	}
};

class SellArticle: CreatureActivity {
public:
	void make() {
		assert (arguments.size() == 1);
		unsigned articleId = arguments.at(0);
		bool canSell = fenotype->articleStocks.at(articleId) >= 1.0;
		if (canSell) {
			float moneyEarned = fenotype->field->market.buyArticleFromClient(
					creature->getId(), articleId, 1.0);
			fenotype->wallet += moneyEarned;
		}
	}
};

CreatureActivityThread::CreatureActivityThread(Creature *creature) :
	QThread() {
	this->creature = creature;
}

CreatureActivityThread::~CreatureActivityThread() {
	// TODO Auto-generated destructor stub
}

void CreatureActivityThread::run() {
	// Wyznacz czynność do wykonania i
	this->creature->doAllActivities();
}
