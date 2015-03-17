#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "trajectorywindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QAction *openAct;
    QAction *exitAct;
    QAction *toggleTranslateAct;
    QAction *takeSnapshotAct;

    TrajectoryWindow * m_trajectoryWidget;

    QMenu *fileMenu;

    void createActions();
    void createMenus();

public:
    MainWindow();
    MainWindow(TrajectoryWindow &tw);
    ~MainWindow();

    void wireConnections();

private slots:
    void takeSnapshot();
};

#endif // MAINWINDOW_H
