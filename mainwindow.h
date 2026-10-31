#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "routemanager.h"
#include <QTableWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QComboBox* comboDepart;
    QComboBox* comboArrivee;
    QPushButton* boutonCalculer;
    QLabel* labelResultat;
    QTableWidget* tableauDistances;

    RouteManager* manager;

private slots:
    void auClicCalculer();

};
#endif // MAINWINDOW_H
