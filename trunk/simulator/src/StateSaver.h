/*
 * StateSaver.h
 *
 *  Created on: 2011-10-12
 *      Author: wojtas
 */

#ifndef STATESAVER_H_
#define STATESAVER_H_
#include <QtCore/qvariant.h>
#include <log4cxx/logger.h>
#include "model/World.h"
class StateSaver {
public:
	StateSaver(World *world, std::string dbName);
	void save ();
	virtual ~StateSaver();
private:

	bool executeQuery (std::string query, std::vector<QVariant> params);
	static LoggerPtr logger;
};

#endif /* STATESAVER_H_ */
