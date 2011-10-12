/*
 * StateSaver.cpp
 *
 *  Created on: 2011-10-12
 *      Author: wojtas
 */

#include "StateSaver.h"

#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>
#include <QtSql/qsqlerror.h>
LoggerPtr StateSaver::logger(Logger::getLogger("stateSaver"));

StateSaver::StateSaver(World *world, std::string dbName) {
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	if (db.isValid())
		db.setDatabaseName(dbName.c_str());
	else {
		LOG4CXX_ERROR(logger, " Database  QSQLITE not valid");
	}

	if (!db.open()) {
		LOG4CXX_ERROR(logger, " Cannot open database  " << dbName.c_str());
	}
}

StateSaver::~StateSaver() {
}

void StateSaver::save() {
	std::vector<QVariant> params;
	params.push_back(QVariant(1));
	bool
			ok =
					executeQuery(
							"insert into worlds_snapshots (world_id, time) values (:world_id, date('now'))",
							params);
	if (ok) {
		//QVariant i = insWordlSnapshot.lastInsertId();
		//LOG4CXX_DEBUG(logger, " WORLD_RUN_ID = " << i.toInt());
	}
}

bool StateSaver::executeQuery(std::string query, std::vector<QVariant> params) {

	QSqlQuery sqlQuery;
	sqlQuery.prepare(query.c_str());
	for (int i = 0; i < params.size(); i++) {
		sqlQuery.bindValue(i, params.at(i));
	}

	if (!sqlQuery.exec()) {
		LOG4CXX_ERROR (logger, "Error executing query " << query << " : " << sqlQuery.lastError().text().toStdString());
		return false;
	}
	return true;
}
