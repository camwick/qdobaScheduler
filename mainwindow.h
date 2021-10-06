#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAdd_triggered();

    void on_actionRemove_triggered();

    void on_actionScheduler_triggered();

    void on_empRemoveBtn_clicked();

    void on_empAddBtn_clicked();

    void on_empAddLNLineEdit_returnPressed();

    void on_scheduleSetOff_clicked();

    void on_schedulePrint_clicked();

    void on_empAddFNLineEdit_returnPressed();

    void on_actionMass_Add_triggered();

    void on_massEmpAddBtn_clicked();

    void on_scheduleClearCells_clicked();

    void on_setHighlightBtn_clicked();

    void on_radioButton1_toggled(bool checked);

    void on_radioButton2_toggled(bool checked);

    void on_radioButton3_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QColor color;

    void updateRemoveComboBox();
    void updateScheduler();
    void saveSchedule();
    void addEmployee(QString first, QString last);
};
#endif // MAINWINDOW_H
