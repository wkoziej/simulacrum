/*
 * Activity.cpp
 *
 *  Created on: 20-10-2011
 *      Author: wojtas
 */

#include "Activity.h"
#include <assert.h>
#include <algorithm>
#include "Creature.h"
#include "Field.h"
#include "Recipe.h"
//#include "Market.h"
//#include "Creature.h"
#include "World.h"
//#include "Article.h"
LoggerPtr Activity::logger(Logger::getLogger("activity"));
std::vector<std::string> Activity::names;
std::vector<bool> Activity::argumentFlag;

Activity::Activity(std::string activityId) :
		id(activityId) {

}

void Activity::init() {
	if (Activity::names.empty()) {

		const char * activitiesNames[] = { "goUp", "goDown", "goLeft",
				"goRight", "rest", "produce", "collect", "leave", "sell", "buy",
				"check", "eat" };
		bool hasArg[] = { false, false, false, false, false, true, true, true,
				true, true, true, true, };

		int i;
		for (i = 0; i < Activity::COUNT; i++) {
			names.push_back(activitiesNames[i]);
			argumentFlag.push_back(hasArg[i]);
		}
		// No thread save, so check :)
		assert(Activity::names.size() == Activity::COUNT);
	}
}

Activity::~Activity() {
}

int Activity::getIndex(Activity::Kind kind) {
	return (int) kind;
}

int Activity::getIndex(std::string name) {
	std::vector<std::string>::iterator activityIterator = std::find(
			names.begin(), names.end(), name);
	assert(activityIterator != names.end());
	return activityIterator - names.begin();
}

std::string Activity::getName(Activity::Kind kind) {
	return getName(getIndex(kind));
}

std::string Activity::getName(int index) {
	assert(index >= 0 && index < Activity::COUNT);
	return names.at(index);
}

bool Activity::hasArgument(int index) {
	assert(index >= 0 && index < Activity::COUNT);
	return argumentFlag.at(index);
}

CreatureActivity::CreatureActivity(Creature *creature,
		UnsignedVector &arguments, std::string activityId) :
		Activity(activityId) {
	this->creature = creature;
	this->arguments.insert(this->arguments.begin(), arguments.begin(),
			arguments.end());
	// TODO Trzeba pamiętać  o uaktualnianiu wartości osobnika
	//this->creature->evaluate(gaTrue);
}

class Arg0CreatureActivity: public CreatureActivity {
public:
	Arg0CreatureActivity(Creature *creature, UnsignedVector &arguments, std::string activityId) :
			CreatureActivity(creature, arguments, activityId) {
	}
	void make() {
		LOG4CXX_DEBUG(logger,
				"creature " << creature->getId().toStdString() << " T:" << creature << " starting to " << activityId());
		// One activity per
		makeActvity();
		LOG4CXX_DEBUG(logger,
				"creature " << creature->getId().toStdString() << " T:" << creature << " ended  " << activityId());

	}
protected:
	virtual void makeActvity() =0;
};

class MoveActivity: public Arg0CreatureActivity {
public:
	MoveActivity(Creature *creature, UnsignedVector &arguments) :
			Arg0CreatureActivity(creature, arguments, "move") {
	}
protected:
	virtual void makeActvity() {
		unsigned X = creature->getX();
		unsigned Y = creature->getY();
		calculateNewCoordinates(X, Y);
		creature->move(X, Y);
	}

	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) = 0;
};

class GoUpActivity: public MoveActivity {
public:
	GoUpActivity(Creature *creature, UnsignedVector &arguments) :
			MoveActivity(creature, arguments) {
	}
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		if (y == 0) {
			y = World::HEIGHT - 1;
		} else {
			y--;
		}
	}
};

class GoDownActivity: public MoveActivity {
public:
	GoDownActivity(Creature *creature, UnsignedVector &arguments) :
			MoveActivity(creature, arguments) {
	}
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		if (y == World::HEIGHT - 1) {
			y = 0;
		} else {
			y++;
		}
	}
};

class GoRightActivity: public MoveActivity {
public:
	GoRightActivity(Creature *creature, UnsignedVector &arguments) :
			MoveActivity(creature, arguments) {
	}
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		if (x == World::WIDTH - 1) {
			x = 0;
		} else {
			x++;
		}
	}
};

class GoLeftActivity: public MoveActivity {
public:
	GoLeftActivity(Creature *creature, UnsignedVector &arguments) :
			MoveActivity(creature, arguments) {
	}
protected:
	virtual void calculateNewCoordinates(unsigned &x, unsigned &y) {
		if (x == 0) {
			x = World::WIDTH - 1;
		} else {
			x--;
		}
	}
};

class RestActivity: public Arg0CreatureActivity {
public:
	RestActivity(Creature *creature, UnsignedVector &arguments) :
			Arg0CreatureActivity(creature, arguments, "rest") {
	}
protected:
	virtual void makeActvity() {
		creature->rest();
	}
};

class Arg1CreatureActivity: public CreatureActivity {
public:
	Arg1CreatureActivity(Creature *creature, UnsignedVector &arguments, std::string activityId) :
			CreatureActivity(creature, arguments, activityId) {
	}

	void make() {
		assert(arguments.size() == 1);
		unsigned articleId = arguments.at(0);
		makeActvity(articleId);
	}
protected:
	virtual void makeActvity(unsigned articleId) =0;
};

class ProduceArticleActivity: public Arg1CreatureActivity {
public:
	ProduceArticleActivity(Creature *creature, UnsignedVector &arguments) :
			Arg1CreatureActivity(creature, arguments, "produce") {
	}
protected:
	void makeActvity(unsigned articleId) {
		const Recipe *recipe = creature->getField()->getRecipe(articleId);
		bool recipeExists = recipe != NULL;
		if (recipeExists) {
			UnsignedVector ingredients = recipe->getIngredientsVector();
			creature->produce(articleId, ingredients);
		}
	}
};

class CollectArticleActivity: public Arg1CreatureActivity {
public:
	CollectArticleActivity(Creature *creature, UnsignedVector &arguments) :
			Arg1CreatureActivity(creature, arguments, "collect") {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->collect(articleId);
	}
};

class LeaveArticleActivity: public Arg1CreatureActivity {
public:
	LeaveArticleActivity(Creature *creature, UnsignedVector &arguments) :
			Arg1CreatureActivity(creature, arguments, "leave") {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->leave(articleId);
	}
};

class SellArticleActivity: public Arg1CreatureActivity {
public:
	SellArticleActivity(Creature *creature, UnsignedVector &arguments) :
			Arg1CreatureActivity(creature, arguments, "sell") {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->sell(articleId);
	}
};

class BuyArticleActivity: public Arg1CreatureActivity {
public:
	BuyArticleActivity(Creature *creature, UnsignedVector &arguments) :
			Arg1CreatureActivity(creature, arguments, "buy") {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->buy(articleId);
	}
};

class CheckArticleActivity: public Arg1CreatureActivity {
public:
	CheckArticleActivity(Creature *creature, UnsignedVector &arguments) :
			Arg1CreatureActivity(creature, arguments, "check") {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->check(articleId);
	}
};

class EatArticleActivity: public Arg1CreatureActivity {
public:
	EatArticleActivity(Creature *creature, UnsignedVector &arguments) :
			Arg1CreatureActivity(creature, arguments, "eat") {
	}
protected:
	void makeActvity(unsigned articleId) {
		creature->eat(articleId);
	}
};

CreatureActivity *CreatureActivity::create(Creature *creature,
		unsigned activityGenIndex) {
	UnsignedVector parameters;
	CreatureActivity *activity;

	parameters.push_back(creature->gene(activityGenIndex + 1));
	Kind activityKind = (Kind) creature->gene(activityGenIndex);

	switch (activityKind) {
	case GoUp:
		activity = new GoUpActivity(creature, parameters);
		break;
	case GoDown:
		activity = new GoDownActivity(creature, parameters);
		break;
	case GoLeft:
		activity = new GoLeftActivity(creature, parameters);
		break;
	case GoRight:
		activity = new GoRightActivity(creature, parameters);
		break;
	case Rest:
		activity = new RestActivity(creature, parameters);
		break;
	case ProduceArticle:
		activity = new ProduceArticleActivity(creature, parameters);
		break;
	case CollectArticle:
		activity = new CollectArticleActivity(creature, parameters);
		break;
	case LeaveArticle:
		activity = new LeaveArticleActivity(creature, parameters);
		break;
	case SellArticle:
		activity = new SellArticleActivity(creature, parameters);
		break;
	case BuyArticle:
		activity = new BuyArticleActivity(creature, parameters);
		break;
	case CheckArticle:
		activity = new CheckArticleActivity(creature, parameters);
		break;
	case EatArticle:
		activity = new EatArticleActivity(creature, parameters);
		break;
	default:
		assert(false);
		break;
	}
	return activity;
}

ActivityList CreatureActivity::getCreatureActivities(Creature *creature) {
	ActivityList activities;
	CreatureActivity *activity = NULL;
	int activityGenIndex = 0;
	int numberOfActivities = creature->getActivitiesCount();
	int stopIndex = numberOfActivities * 2;
	for (; activityGenIndex < stopIndex;) {
		activity = CreatureActivity::create(creature, activityGenIndex);
		activities.push_back(activity);
		activityGenIndex += 2;
	}
	return activities;
}
