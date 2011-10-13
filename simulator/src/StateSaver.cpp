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
	executeQuery(
			"insert into worlds_runs (world_id, date_start) values (:1, datetime('now'));",
			params, runId);

}

StateSaver::~StateSaver() {
}

void StateSaver::save() {
	std::vector<QVariant> params;
	params.push_back(runId);
	int worldSnapshotId;
	executeQuery(
			"insert into worlds_snapshots (world_run_id, snapshot_time) values (:1, datetime('now'));",
			params, worldSnapshotId);
	FieldsMatrix::iterator i = world->fields.begin();
	for (; i != world->fields.end(); i++) {
		FieldsVector::iterator j = i->begin();
		for (; j != i->end(); j++) {
			Field *field = *j;
			int fieldSnapshotId;
			params.clear();
			params.push_back(worldSnapshotId);
			params.push_back(field->getMoveLag());
			executeQuery(
					"insert into fields_snapshots (world_snapshot_id, lag) values (:1, :2);",
					params, fieldSnapshotId);
			// PRODUKTY
			int tmp;
			for (int productIndex = 0; productIndex < World::NO_OF_PRODUCTS; productIndex++) {
				params.clear();
				params.push_back(fieldSnapshotId);
				params.push_back(productIndex);
				params.push_back(field->productPrice(productIndex));
				params.push_back(field->productStock(productIndex));
				executeQuery(
						"insert into fields_products_snapshots (field_snapshot_id, product_id, price, quant) "
							" values (:1, :2, :3, :4);", params, tmp);

			}
			// ZASOBY
			for (int resourceIndex = 0; resourceIndex < World::NO_OF_RESOURCES; resourceIndex++) {
				params.clear();
				params.push_back(fieldSnapshotId);
				params.push_back(resourceIndex);
				params.push_back(field->resourcePrice(resourceIndex));
				params.push_back(field->resourceQuantitiesVector().at(
						resourceIndex));
				executeQuery(
						"insert into fields_resources_snapshots (field_snapshot_id, resource_id, price, amount) "
							" values (:1, :2, :3, :4);", params, tmp);
			}

			// POPULACJE
			PopulationsMap::iterator i = field->populations.begin();
			if (i != field->populations.end()) {
				CreaturesPopulation *population = i->second;
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
							(Creature *) &(population->individual(i,
									GAPopulation::RAW));
					int creatureSnapshotId;
					params.clear();
					params.push_back(populationSnapshotId);
					params.push_back(creature->getFenotype()->yearsOld);
					params.push_back(creature->getFenotype()->objectiveValue);
					params.push_back(creature->genomeStr().c_str());
					executeQuery(
							"insert into creatures_snapshots (population_snapshot_id, age, objective_value, genome) "
								" values (:1, :2, :3, :4);", params,
							creatureSnapshotId);

					int tmp;
					for (int productIndex = 0; productIndex
							< World::NO_OF_PRODUCTS; productIndex++) {
						params.clear();
						params.push_back(creatureSnapshotId);
						params.push_back(productIndex);
						params.push_back(
								creature->getFenotype()->createdProductQuants.at(
										productIndex));
						params.push_back(creature->getNeedOfProductRatio(
								productIndex));
						executeQuery(
								"insert into creatures_products_snapshots (creature_snapshot_id, product_id, produced, needs) "
									" values (:1, :2, :3, :4);", params, tmp);

					}

					for (int resourceIndex = 0; resourceIndex
							< World::NO_OF_RESOURCES; resourceIndex++) {

						params.clear();
						params.push_back(creatureSnapshotId);
						params.push_back(resourceIndex);
						params.push_back(
								creature->getFenotype()->usedResourcesQuants.at(
										resourceIndex));
						executeQuery(
								"insert into creatures_resources_snapshots (creature_snapshot_id, resource_id, used) "
									" values (:1, :2, :3);", params, tmp);
					}
				}
			}

		}
	}

}

bool StateSaver::executeQuery(std::string query, std::vector<QVariant> params,
		int &lastInsertId) {
	QSqlQuery sqlQuery;
	sqlQuery.prepare(query.c_str());
	for (int i = 0; i < params.size(); i++) {
		sqlQuery.bindValue(i, params.at(i));
	}
	if (!sqlQuery.exec()) {
		LOG4CXX_ERROR (logger, "Error executing query " << query << " : " << sqlQuery.lastError().text().toStdString());
		return false;
	} else {
		QVariant i = sqlQuery.lastInsertId();
		lastInsertId = i.toInt();
		LOG4CXX_DEBUG(logger, "query = " << query << ", LAST_INSERT_ID = " << lastInsertId);
	}
	return true;
}
