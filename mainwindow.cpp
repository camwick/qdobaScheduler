#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui -> setupUi(this);

    // path of database file
    QString path = QCoreApplication::applicationDirPath() + "/qdobaEmployees.db";

    // connecting to database
    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER)){
        db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName(path);
    }

    if(!db.open())
        qWarning() << "ERROR: " << db.lastError();

    // update SQL dependent window elements
    updateScheduler();
    updateRemoveComboBox();

    // set stacked widget to schedule page
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
    // structuring name strings
    QString fullName = ui -> empRemoveComboBox -> currentText();
    QStringList nameParts = fullName.split(" ");

    // get record of employee to delete
    QSqlQuery query;
    query.prepare("SELECT * FROM employees WHERE lastName = ?");
    query.addBindValue(nameParts[1]);
    if(!query.exec())
        qWarning() << "empRemoveBtn before While ERROR: " << query.lastError();

    // delete employee record
    while(query.next()){
        // if statement catches case of same last names
        if(query.value(1) == nameParts[0]){
            QString foreignKey = query.value(0).toString();

            query.prepare("DELETE FROM employees where firstName = ?");
            query.addBindValue(nameParts[0]);
            if(!query.exec())
                qWarning() << "empRemoveBtn in While ERROR: " << query.lastError();

            query.prepare("DELETE FROM schedule where id = ?");
            query.addBindValue(foreignKey);
            if(!query.exec())
                qWarning() << "empRemoveBtn in While ERROR: " << query.lastError();
        }
    }

    // update SQL dependent window elements
    ui -> empRemoveComboBox -> clear();
    updateRemoveComboBox();
    updateScheduler();
}

void MainWindow::on_empAddBtn_clicked()
{
    // getting name elements
    QString first = ui -> empAddFNLineEdit -> text().trimmed();
    QString last = ui -> empAddLNLineEdit -> text().trimmed();

    // catching attempt of empty table record creation
    if(first == "" || last == ""){
        QMessageBox warning;
        warning.setIcon(QMessageBox::Warning);
        warning.setText("Both text fields must be filled in order to add an employee to records.");
        warning.exec();
    }
    else{
        QSqlQuery query;

        // insert new employee records
        query.prepare("INSERT INTO employees (firstName, lastName) VALUES (:first, :last)");
        query.bindValue(":first", first);
        query.bindValue(":last", last);
        query.exec();

        // get employee id
        query.prepare("SELECT * FROM employees WHERE lastName = ?");
        query.addBindValue(last);
        if(!query.exec())
            qWarning() << "Insert employee Error: " << query.lastError();

        // insert new employee and schedule records
        while(query.next()){
            if(query.value(1) == first){
                QString foreignKey = query.value(0).toString();
                query.prepare("INSERT INTO schedule (id) VALUES (:id)");
                query.bindValue(":id", foreignKey);
                if(!query.exec())
                    qWarning() << "Insert new schedule Error: " << query.lastError();
            }
        }

        if(!query.isActive())
            qWarning() << "ERROR: " << query.lastError().text();

        // clear text fields and update window elements dependent of SQL records
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

    // get names from database
    QSqlQuery query;
    QString data;
    query.exec("SELECT firstName, lastName FROM employees ORDER BY lastName ");

    // update combobox
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

    // get names from database
    QSqlQuery query;
    query.exec("SELECT firstName, lastName FROM employees ORDER BY lastName");
    QStringList employees = {};
    int counter = 0;

    // creating name list
    while(query.next()){
        employees.append(query.value(0).toString() + " " + query.value(1).toString());
        counter++;

        if(!query.isActive())
            qWarning() << "ERROR: " << query.lastError().text();
    }


    // update tableWidget headers
    ui -> tableWidget -> setRowCount(counter);
    ui -> tableWidget -> setVerticalHeaderLabels(employees);
}



