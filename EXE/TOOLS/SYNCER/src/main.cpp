#include <QApplication>
#include "syncer.h"

int main(int argc, char *argv[])
{
    QApplication EditorApp(argc, argv);
    Syncer mainWindow;

    return EditorApp.exec();
}
