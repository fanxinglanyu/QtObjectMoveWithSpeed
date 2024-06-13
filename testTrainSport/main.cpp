#include "main_interface.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainInterface w;

    w.showMaximized();
    //w.show();

    return a.exec();
}
