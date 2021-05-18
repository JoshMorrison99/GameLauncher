#include "gamelauncher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameLauncher w;
    w.show();
    return a.exec();
}
