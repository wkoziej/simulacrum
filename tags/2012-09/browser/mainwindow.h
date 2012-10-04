#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <log4cxx/logger.h>
#include <QtGui/QMainWindow>
#include <QtGui/QTableView>
#include <qt4/QtSql/QSqlTableModel>

using namespace log4cxx;

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow {
Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void setUpTable(QTableView *tableView, QString sqlTableName,
			QSqlTableModel *&model);
public slots:
	void changeWorldRun(const QModelIndex &index);
	void changeWorldSnapshot(const QModelIndex &index);
	void changeFieldSnapshot(const QModelIndex &index);
	void changePopulationSnapshot(const QModelIndex &index);
	void changeCreatureSnapshot(const QModelIndex &index);
	void refreshData();
private:
	Ui::MainWindow *ui;

	QSqlTableModel *worldRunsModel;
	QSqlTableModel *worldSnapshotModel;
	QSqlTableModel *fieldSnapshotModel;
	QSqlTableModel *fieldArticlesSnapshotModel;
	QSqlTableModel *creatureArticlesSnapshotModel;
	QSqlTableModel *fieldPopulationSnapshotModel;
	QSqlTableModel *populationCreatureSnapshotModel;
	static LoggerPtr logger;

	void changeMasterId(QSqlTableModel *master, const QModelIndex &index,
			QSqlTableModel *detail, QString masterColumnName, QTableView *datailTableView);
};
#endif // MAINWINDOW_H
