#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>

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

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

    void updateRemoveComboBox();
};
#endif // MAINWINDOW_H
