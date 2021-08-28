#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString path = QCoreApplication::applicationDirPath() + "/qdobaEmployees.db";

    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER)){
        db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName(path);
    }

    if(!db.open())
        qWarning() << "ERROR: " << db.lastError();

    updateScheduler();
    updateRemoveComboBox();

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
}

void MainWindow::on_actionScheduler_triggered()
{
    ui -> stackedWidget -> setCurrentIndex(2);
}

// window button functions
void MainWindow::on_empRemoveBtn_clicked()
{
    QString fullName = ui -> empRemoveComboBox -> currentText();
    QStringList nameParts = fullName.split(" ");

    qDebug() << nameParts;

    QSqlQuery query;
    query.prepare("SELECT * FROM employees WHERE lastName = ?");
    query.addBindValue(nameParts[1]);
    if(!query.exec())
        qWarning() << "empRemoveBtn before While ERROR: " << query.lastError();

    while(query.next()){
        if(query.value(1) == nameParts[0]){
            query.prepare("DELETE FROM employees where firstName = ?");
            query.addBindValue(nameParts[0]);
            if(!query.exec())
                qWarning() << "empRemoveBtn in While ERROR: " << query.lastError();
        }
    }

    ui -> empRemoveComboBox -> clear();
    updateRemoveComboBox();
    updateScheduler();
}

void MainWindow::on_empAddBtn_clicked()
{
    QString first = ui -> empAddFNLineEdit -> text().trimmed();
    QString last = ui -> empAddLNLineEdit -> text().trimmed();

    if(first == "" || last == ""){
        QMessageBox warning;
        warning.setIcon(QMessageBox::Warning);
        warning.setText("Both text fields must be filled in order to add an employee to records.");
        warning.exec();
    }
    else{
        QSqlQuery query;

        query.prepare("INSERT INTO employees (firstName, lastName) VALUES (:first, :last)");
        query.bindValue(":first", first);
        query.bindValue(":last", last);
        query.exec();

        if(!query.isActive())
            qWarning() << "ERROR: " << query.lastError().text();

        ui -> empAddFNLineEdit -> clear();
        ui -> empAddLNLineEdit -> clear();
        updateRemoveComboBox();
        updateScheduler();
    }
}

// return press functions
void MainWindow::on_empAddLNLineEdit_returnPressed()
{
    MainWindow::on_empAddBtn_clicked();
}

// custom functions
void MainWindow::updateRemoveComboBox()
{
    ui -> empRemoveComboBox -> clear();

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

void MainWindow::updateScheduler()
{
    ui -> tableWidget -> clearContents();

    QSqlQuery query;
    query.exec("SELECT firstName, lastName FROM employees ORDER BY lastName");

    QStringList employees = {};
    int counter = 0;
    while(query.next()){
        employees.append(query.value(0).toString() + " " + query.value(1).toString());
        counter++;

        if(!query.isActive())
            qWarning() << "ERROR: " << query.lastError().text();
    }

    ui -> tableWidget -> setRowCount(counter);
    ui -> tableWidget -> setVerticalHeaderLabels(employees);
}



