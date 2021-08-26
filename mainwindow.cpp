#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER)){
        db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName("C:/Users/cam_w/Desktop/SQLiteStudio/qdobaEmployees.db");
    }

    if(!db.open())
        qWarning() << "ERROR: " << db.lastError();

    ui -> stackedWidget-> setCurrentIndex(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Hotbar trigger actions
void MainWindow::on_actionAdd_triggered()
{
    ui -> stackedWidget -> setCurrentIndex(0);
}

void MainWindow::on_actionRemove_triggered()
{
    ui -> stackedWidget -> setCurrentIndex(1);

    QSqlQuery query;
    QString data;

    query.exec("SELECT firstName, lastName FROM employees ORDER BY lastName ");

    while(query.next()){
        data = query.value(0).toString() + " " + query.value(1).toString();

        ui -> empRemoveComboBox -> addItem(data);

        if(!query.isActive())
            qWarning() << "ERROR: " << query.lastError().text();
    }
}

void MainWindow::on_actionScheduler_triggered()
{
    ui -> stackedWidget -> setCurrentIndex(2);
}

