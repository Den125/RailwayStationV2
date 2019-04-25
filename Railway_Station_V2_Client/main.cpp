#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    if (w.Connect()!=true)
        return false;
    if (w.Login()!=true)
        return false;

    return a.exec();
}
