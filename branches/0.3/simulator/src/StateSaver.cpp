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
#include "model/Market.h"

LoggerPtr StateSaver::logger(Logger::getLogger("stateSaver"));

StateSaver::StateSaver(World *world, std::string dbName) {
	this->world = world;
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

void StateSaver::startSession() {
	std::vector<QVariant> params;
	params.push_back(1);
	QSqlDatabase::database().transaction();

	executeQuery(
			"insert into worlds_runs (world_id, date_start) values (:1, datetime('now'));",
			params, runId);
	std::vector<std::string>::iterator a;
	int tmp;
	for (a = World::ARTICLES.begin(); a < World::ARTICLES.end(); a++) {
		params.clear();
		params.push_back(a->c_str());
		executeQuery("insert into articles (name) values (:1);",
				params, tmp);
	}
	QSqlDatabase::database().commit();
}

StateSaver::~StateSaver() {
}

void StateSaver::save(std::string description) {
	std::vector<QVariant> params;
	params.push_back(runId);
	params.push_back(description.c_str());
	int worldSnapshotId;
	QSqlDatabase::database().transaction();

	executeQuery(
			"insert into worlds_snapshots (world_run_id, snapshot_time, description) values (:1, datetime('now'), :2);",
			params, worldSnapshotId);
	FieldsMatrix::iterator i = world->fields.begin();
	for (; i != world->fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			Field *field = *j;
			int fieldSnapshotId;
			params.clear();
			params.push_back(worldSnapshotId);
			params.push_back(/*field->getMoveLag()*/0);
			executeQuery(
					"insert into fields_snapshots (world_snapshot_id, lag) values (:1, :2);",
					params, fieldSnapshotId);
			// artykuły
			int tmp;
			for (int articleIndex = 0; articleIndex < World::NO_OF_ARTICLES; articleIndex++) {
				params.clear();
				params.push_back(fieldSnapshotId);
				params.push_back(articleIndex + 1);
				params.push_back(field->articleStock(articleIndex));
				params.push_back(field->getMarket()->articleStock(articleIndex));
				params.push_back(field->getMarket()->articleSellPrice(
						articleIndex));
				params.push_back(field->getMarket()->articleBuyPrice(
						articleIndex));
				params.push_back(field->getMarket()->queriesCount(
									articleIndex));
				executeQuery(
						"insert into fields_articles_snapshots (field_snapshot_id, article_id, field_stock, market_stock, sell_price, buy_price, queries_count) "
							" values (:1, :2, :3, :4, :5, :6, :7);", params, tmp);

			}
			// POPULACJE
			std::list<CreaturesPopulation *> populations =
					field->getPopulations();
			std::list<CreaturesPopulation *>::iterator p = populations.begin();
			for (; p != populations.end(); p++) {
				CreaturesPopulation *population = *p;
				int populationSnapshotId;
				params.clear();
				params.push_back(fieldSnapshotId);
				std::wstring popName = population->getName();
				std::string name(popName.length(), ' ');
				std::copy(popName.begin(), popName.end(), name.begin());
				params.push_back(name.c_str());
				executeQuery(
						"insert into populations_snapshots (field_snapshot_id, species) values (:1, :2);",
						params, populationSnapshotId);
				int popSize = population->size();
				for (int i = 0; i < popSize; i++) {
					Creature * creature =
							(Creature *) &(population->individual(i));
					int creatureSnapshotId;
					params.clear();
					params.push_back(populationSnapshotId);
					params.push_back(creature->getAge());
					params.push_back(creature->score());
					params.push_back(creature->getWallet ());
					params.push_back((creature->getId().toStdString() + " : "
							+ creature->genomeStr()).c_str());
					executeQuery(
							"insert into creatures_snapshots (population_snapshot_id, age, objective_value, wallet, genome) "
								" values (:1, :2, :3, :4, :5);", params,
							creatureSnapshotId);
					int tmp;
					for (int articleIndex = 0; articleIndex
							< World::NO_OF_ARTICLES; articleIndex++) {
						params.clear();
						params.push_back(creatureSnapshotId);
						params.push_back(articleIndex + 1);
						params.push_back(
								creature->getArticleStock(articleIndex));
						params.push_back(creature->getArticleQuantChange(
								articleIndex));
						executeQuery(
								"insert into creatures_articles_snapshots (creature_snapshot_id, article_id, stock, changed) "
									" values (:1, :2, :3, :4);", params, tmp);

					}
				}
			}
		}
	}
	QSqlDatabase::database().commit();
}

bool StateSaver::executeQuery(std::string query, std::vector<QVariant> params,
		int &lastInsertId) {
	QSqlQuery sqlQuery;
	sqlQuery.prepare(query.c_str());
	for (int i = 0; i < params.size(); i++) {
		sqlQuery.bindValue(i, params.at(i));
	}

	if (!sqlQuery.exec()) {
		QSqlDatabase::database().rollback();
		LOG4CXX_ERROR (logger, "Error executing query " << query << " : " << sqlQuery.lastError().text().toStdString());
		exit(-1);
	} else {
		QVariant i = sqlQuery.lastInsertId();
		lastInsertId = i.toInt();
		LOG4CXX_DEBUG(logger, "query = " << query << ", LAST_INSERT_ID = " << lastInsertId);
	}
	return true;
}
