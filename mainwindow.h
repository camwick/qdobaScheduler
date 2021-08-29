#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTextDocument>

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

    void on_scheduleClearCells_clicked();

    void on_scheduleSetOff_clicked();

    void on_schedulePrint_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

    void updateRemoveComboBox();
    void updateScheduler();
    void saveSchedule();
};
#endif // MAINWINDOW_H
