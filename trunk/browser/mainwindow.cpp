#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qt4/QtSql/QSqlDatabase>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlrecord.h>

LoggerPtr MainWindow::logger(Logger::getLogger("browser"));

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	if (db.isValid())
		db.setDatabaseName("../simulator/project-world.db");
	if (!db.isValid() || !db.open()) {
		// Give error message, e.g., using QMessageBox
		// QMessageBox::information("Error", "Cannot open database");
		qDebug("Cannot open database");
		return;
	}

	setUpTable(ui->worldRunsTableView, "v_worlds_runs", worldRunsModel);
	worldRunsModel->setSort(2, Qt::DescendingOrder);

	setUpTable(ui->worldSnapshotTableView, "v_worlds_snapshots",
			worldSnapshotModel);
	setUpTable(ui->fieldSnapshotTableView, "v_fields_snapshots",
			fieldSnapshotModel);
	setUpTable(ui->populationCreaturesTableView, "v_creatures_snapshots",
			populationCreatureSnapshotModel);
	populationCreatureSnapshotModel->setSort(4, Qt::DescendingOrder);
	setUpTable(ui->fieldPopulationsTableView, "v_populations_snapshots",
			fieldPopulationSnapshotModel);
	setUpTable(ui->fieldArticlesTableView, "v_fields_articles_snapshots",
			fieldArticlesSnapshotModel);
	setUpTable(ui->creatureArticlesTableView, "v_creatures_articles_snapshots",
			creatureArticlesSnapshotModel);

	connect(
			ui->worldRunsTableView->selectionModel(),
			SIGNAL(currentRowChanged ( const QModelIndex & , const QModelIndex & )),
			this, SLOT(changeWorldRun(const QModelIndex &)));
	connect(
			ui->worldSnapshotTableView->selectionModel(),
			SIGNAL(currentRowChanged ( const QModelIndex & , const QModelIndex & )),
			this, SLOT(changeWorldSnapshot(const QModelIndex &)));

	connect(
			ui->fieldSnapshotTableView->selectionModel(),
			SIGNAL(currentRowChanged ( const QModelIndex & , const QModelIndex & )),
			this, SLOT(changeFieldSnapshot(const QModelIndex &)));

	connect(
			ui->fieldPopulationsTableView->selectionModel(),
			SIGNAL(currentRowChanged ( const QModelIndex & , const QModelIndex & )),
			this, SLOT(changePopulationSnapshot(const QModelIndex &)));

	connect(
			ui->populationCreaturesTableView->selectionModel(),
			SIGNAL(currentRowChanged ( const QModelIndex & , const QModelIndex & )),
			this, SLOT(changeCreatureSnapshot(const QModelIndex &)));
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(refreshData()));
}

void MainWindow::setUpTable(QTableView *tableView, QString sqlTableName,
		QSqlTableModel *&model) {
	model = new QSqlTableModel();
	model->setTable(sqlTableName);
	model->select();
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	tableView->setModel(model);
	tableView->hideColumn(0);
	tableView->hideColumn(1);
	tableView->resizeColumnsToContents();
	if (!model->select()) {
		LOG4CXX_ERROR (logger, "Error selecting rows: " << model->lastError().text().toStdString());
	}
}

void MainWindow::changeWorldRun(const QModelIndex &index) {
	changeMasterId(worldRunsModel, index, worldSnapshotModel, "world_run_id",
			ui->worldSnapshotTableView);
}

void MainWindow::changeWorldSnapshot(const QModelIndex &index) {
	changeMasterId(worldSnapshotModel, index, fieldSnapshotModel,
			"world_snapshot_id", ui->fieldSnapshotTableView);
}

void MainWindow::changeFieldSnapshot(const QModelIndex &index) {
	LOG4CXX_DEBUG(logger, "changeFieldSnapshot " << " index.row = " << index.row());
	changeMasterId(fieldSnapshotModel, index, fieldPopulationSnapshotModel,
			"field_snapshot_id", ui->fieldPopulationsTableView);
	changeMasterId(fieldSnapshotModel, index, fieldArticlesSnapshotModel,
			"field_snapshot_id", ui->fieldArticlesTableView);
}

void MainWindow::changePopulationSnapshot(const QModelIndex &index) {
	LOG4CXX_DEBUG(logger, "changePopulationSnapshot " << " index.row = " << index.row());
	changeMasterId(fieldPopulationSnapshotModel, index,
			populationCreatureSnapshotModel, "population_snapshot_id",
			ui->populationCreaturesTableView);
}

void MainWindow::changeCreatureSnapshot(const QModelIndex &index) {
	LOG4CXX_DEBUG(logger, "changeCreatureSnapshot " << " index.row = " << index.row());
	changeMasterId(populationCreatureSnapshotModel, index,
			creatureArticlesSnapshotModel, "creature_snapshot_id",
			ui->creatureArticlesTableView);
}

void MainWindow::refreshData() {
	worldRunsModel->select();
}

void MainWindow::changeMasterId(QSqlTableModel *master,
		const QModelIndex &index, QSqlTableModel *detail,
		QString masterColumnName, QTableView *datailTableView) {
	LOG4CXX_DEBUG(logger, "changeMasterId, detail name << " << datailTableView->objectName().toStdString()
			<< " index.row = " << index.row());
	if (index.row() >= 0) {
		QModelIndex pk = master->index(index.row(), 0);
		QString detailCondition = masterColumnName + " = "
				+ pk.data().toString();
		LOG4CXX_DEBUG(logger, "changeMasterId, detailCondition = " << detailCondition.toStdString() );
		detail->setFilter(detailCondition);
		datailTableView->selectRow(0);
	} else if (index.row() == 0) {
		detail->setFilter(QString());
	} else {
		return;
	}
}

MainWindow::~MainWindow() {
	delete ui;
	QSqlDatabase db = QSqlDatabase::database();
	db.close();
}
