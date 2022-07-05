#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include "calib.h"

int main(int argc, char *argv[])
{
    QApplication EditorApp(argc, argv);

    QMainWindow mainwindow;
    mainwindow.resize(1280, 720);
    CALIB calib;
    calib.resize(1280, 720);
    mainwindow.setCentralWidget(&calib);
    mainwindow.show();

    return EditorApp.exec();
}
