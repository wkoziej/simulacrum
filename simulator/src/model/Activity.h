/*
 * Activity.h
 *
 *  Created on: 20-10-2011
 *      Author: wojtas
 */

#ifndef ACTIVITY_H_
#define ACTIVITY_H_
#include <vector>
#include <string>
#include <list>
#include <log4cxx/logger.h>
#include "Types.h"
using namespace log4cxx;
class Activity {
public:
	enum Kind {
		// no argument
		GoUp,
		GoDown,
		GoLeft,
		GoRight,
		Rest,
		// one arguement
		ProduceArticle,
		CollectArticle,
		LeaveArticle,
		SellArticle,
		BuyArticle,
		CheckArticle,
		EatArticle,
		// Size
		COUNT
	};

	Activity(std::string activityId);
	virtual ~Activity();
	static void init ();
	static int getIndex(Activity::Kind kind);
	static int getIndex(std::string name);
	static std::string getName (Activity::Kind kind);
	static std::string getName(int index);
	static bool hasArgument(int index);
	std::string activityId() { return id; };
protected:
	static LoggerPtr logger;
	std::string id;
private:

	static std::vector<std::string> names;
	static std::vector<bool> argumentFlag;
};

class Field;
class Creature;
class CreatureActivity;
typedef std::list<CreatureActivity *> ActivityList;

class CreatureActivity: public Activity {
protected:
	Creature *creature;
	//const Field *field;
	UnsignedVector arguments;
public:
	virtual void make() = 0;
	static ActivityList getCreatureActivities(Creature *creature);
protected:
	static CreatureActivity *create(Creature *creature, unsigned activityIndex);
	CreatureActivity(Creature *creature, UnsignedVector &arguments, std::string activityId);

};
#endif /* ACTIVITY_H_ */
