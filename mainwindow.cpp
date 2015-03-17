#include "mainwindow.h"
#include "trajectorywindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>

MainWindow::MainWindow(TrajectoryWindow &tw) : m_trajectoryWidget(&tw)
{
    createActions();
    createMenus();
}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions() {
    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    toggleTranslateAct = new QAction(tr("&Translate Mode"), this);
    toggleTranslateAct->setShortcut(Qt::Key_T);

    takeSnapshotAct = new QAction(tr("&Save Image"), this);
    takeSnapshotAct->setShortcut(tr("Ctrl-S"));
    connect(takeSnapshotAct, SIGNAL(triggered()), m_trajectoryWidget, SLOT(takeSnapshot()));
}

void MainWindow::wireConnections() {
    connect(toggleTranslateAct, SIGNAL(triggered()), centralWidget(), SLOT(toggleTranslate()));
    //connect(takeSnapshotAct, SIGNAL(triggered()), centralWidget(), SLOT(takeSnapshot()));
}

void MainWindow::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(exitAct);
    fileMenu->addAction(toggleTranslateAct);
    fileMenu->addAction(takeSnapshotAct);

    menuBar()->addMenu(fileMenu);
}

void MainWindow::takeSnapshot() {
    setUpdatesEnabled(false);
    QImage snapshot = m_trajectoryWidget->grabFramebuffer();
    snapshot.save("test.jpeg");
    setUpdatesEnabled(true);
}
