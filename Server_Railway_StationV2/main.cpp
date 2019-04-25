#include "Gui.hpp"
#include "autorization.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gui w;
    w.show();
    if (w.ConnectDatabase()!=true)
        return false;
    if (w.Login()!=true)
        return false;
    return a.exec();
}
