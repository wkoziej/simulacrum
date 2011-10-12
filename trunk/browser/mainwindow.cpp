#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"
#include <qt4/QtSql/QSqlDatabase>
#include <qt4/QtSql/QSqlTableModel>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    if (db.isValid())
        db.setDatabaseName("/home/wojtas/devel/simulacrum/project-world.db");
    if (!db.isValid() || !db.open()) {
        // Give error message, e.g., using QMessageBox
        // QMessageBox::information("Error", "Cannot open database");
        qDebug("Cannot open database");
        return;
    }


    QSqlTableModel *model = new QSqlTableModel();
    model->setTable("v_worlds_runs");
    model->select();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    //model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

    ui->worldRunsTableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}
