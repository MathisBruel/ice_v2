#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include "sync.h"

int main(int argc, char *argv[])
{
    QApplication EditorApp(argc, argv);

    QMainWindow mainwindow;
    mainwindow.resize(1280, 720);
    SYNC sync;
    sync.resize(1280, 720);
    mainwindow.setCentralWidget(&sync);
    mainwindow.show();

    return EditorApp.exec();
}
