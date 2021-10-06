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

    // set default green color highlighting
    MainWindow::on_radioButton1_toggled(true);
}

MainWindow::~MainWindow()
{
    // saving tableWidget contents to database
    MainWindow::on_actionSave_Schedule_triggered();

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

void MainWindow::on_actionMass_Add_triggered()
{
    ui -> stackedWidget -> setCurrentIndex(3);
}

void MainWindow::on_actionSave_Schedule_triggered()
{
    QSqlQuery query;
    QStringList employeeIDs = {};
    QStringList tableValues = {};

    if(!query.exec("SELECT id FROM employees ORDER BY lastName"))
        qWarning() << "saveSchedule get IDs Error: " << query.lastError();

    while(query.next())
        employeeIDs.append(query.value(0).toString());

    for(int i = 0; i < ui -> tableWidget -> rowCount(); i++){
        for(int j = 0; j < 7; j++){
            if(ui -> tableWidget -> item(i, j) == nullptr){
                tableValues.append(NULL);
            }
            else{
                tableValues.append(ui -> tableWidget -> item(i, j) -> text());
            }
        }
    }

    query.prepare("UPDATE schedule SET monday = :mon, tuesday = :tues, wednesday = :wednes, thursday = :thurs, friday = :fri, saturday = :sat, sunday = :sun WHERE id = :id");
    for(int i = 0; i < ui -> tableWidget -> rowCount(); i++){
        int j = 0;

        query.bindValue(":id", employeeIDs[i]);
        query.bindValue(":mon", tableValues[7 * i + j++]);
        query.bindValue(":tues", tableValues[7 * i + j++]);
        query.bindValue(":wednes", tableValues[7 * i + j++]);
        query.bindValue(":thurs", tableValues[7 * i + j++]);
        query.bindValue(":fri", tableValues[7 * i + j++]);
        query.bindValue(":sat", tableValues[7 * i + j++]);
        query.bindValue(":sun", tableValues[7 * i + j++]);

        if(!query.exec())
            qWarning() << "saveSchedule update Error: " << query.lastError();
    }
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

            break;
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
        MainWindow::addEmployee(first, last);

        // clear text fields and update window elements dependent of SQL records
        ui -> empAddFNLineEdit -> clear();
        ui -> empAddLNLineEdit -> clear();
        updateRemoveComboBox();
    }
}

void MainWindow::on_scheduleClearCells_clicked()
{
    // clears content from all the cells
    for(int i = 0; i < ui -> tableWidget -> rowCount(); i++){
        for(int j = 0; j < 7; j++){
            ui -> tableWidget -> item(i, j) -> setText("");
            ui -> tableWidget -> item(i, j) -> setBackground(Qt::NoBrush);
        }
    }

    /* Note:
     * clearContent() cannot be used due to MainWindow::on_pushButton_2_clicked() below.
     * The selectedItems() function utlized by on_pushButton_2_clicked() needs content
     * in the cell to work correctly. clearContent() sets all cells to nullptr and won't
     * be returned with selectedItems().
     */
}

void MainWindow::on_scheduleSetOff_clicked()
{
    QList<QTableWidgetItem *> selectedValues = ui -> tableWidget -> selectedItems();

    for(int i = 0; i < selectedValues.size(); i++){
        selectedValues[i] -> setText("Off");
        selectedValues[i] -> setBackground(Qt::red);
    }
}

void MainWindow::on_massEmpAddBtn_clicked()
{
    // get text from QTextEdit
    QString plainText = ui -> massEmpAddTextEdit -> toPlainText();
    QStringList empNames = plainText.split("\n");

    // split empNames to useable name parts
    QStringList nameParts;
    for(int i = 0; i < empNames.length(); i++){
        QStringList temp = empNames[i].split(" ");

        for(int j = 0; j < temp.length(); j++){
            temp[j] = temp[j].trimmed();

            if(temp[j] == "")
                continue;
            else
                nameParts.append(temp[j]);
        }
    }

    // run queries
    for(int i = 0; i < nameParts.length(); i+=2){
        MainWindow::addEmployee(nameParts[i], nameParts[i+1]);
    }

    // clear text fields and update window elements dependent of SQL records

    ui -> massEmpAddTextEdit -> clear();
}

void MainWindow::on_setHighlightBtn_clicked()
{
    QList<QTableWidgetItem *> selectedValues = ui -> tableWidget -> selectedItems();

    for(int i = 0; i < selectedValues.size(); i++){
        selectedValues[i] -> setBackground(color);
    }
}

// return press functions
void MainWindow::on_empAddLNLineEdit_returnPressed()
{
    MainWindow::on_empAddBtn_clicked();
}

void MainWindow::on_empAddFNLineEdit_returnPressed()
{
    MainWindow::on_empAddBtn_clicked();
}

// radio button functions
void MainWindow::on_radioButton1_toggled(bool checked)
{
    if(checked)
        color = Qt::green;
}


void MainWindow::on_radioButton2_toggled(bool checked)
{
    if(checked)
        color = Qt::cyan;
}


void MainWindow::on_radioButton3_toggled(bool checked)
{
    if(checked)
        color = Qt::yellow;
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
    query.exec("SELECT id, firstName, lastName FROM employees ORDER BY lastName");
    QStringList employees = {};
    QStringList employeeIDs = {};
    int counter = 0;

    // creating name list
    while(query.next()){
        employeeIDs.append(query.value(0).toString());
        employees.append(query.value(1).toString() + " " + query.value(2).toString());
        counter++;

        if(!query.isActive())
            qWarning() << "ERROR: " << query.lastError().text();
    }

    // update tableWidget headers
    ui -> tableWidget -> setRowCount(counter);
    ui -> tableWidget -> setVerticalHeaderLabels(employees);

    // update values from db to table
    query.prepare("SELECT monday, tuesday, wednesday, thursday, friday, saturday, sunday FROM schedule WHERE id = ?");
    for(int i = 0; i < ui -> tableWidget -> rowCount(); i++){
        query.addBindValue(employeeIDs[i]);

        if(!query.exec())
            qWarning() << "updateScheduler select * Error: " << query.lastError();

        while(query.next())
            for(int j = 0; j < 7; j++){
                ui -> tableWidget -> setItem(i, j, new QTableWidgetItem(query.value(j).toString()));

                if(query.value(j).toString() == "Off")
                    ui -> tableWidget -> item(i, j) -> setBackground(Qt::red);
            }
    }
}

void MainWindow::addEmployee(QString first, QString last)
{
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

            break;
        }
    }

    if(!query.isActive())
        qWarning() << "ERROR: " << query.lastError().text();

    updateScheduler();
}
