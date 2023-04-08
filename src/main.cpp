#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

/*
 *
 *  [~] Assassin feature
 *  [-] QChart
 *  [+] Date Validation
 *  [+] Codestyle
 *  [+] Complete Model thingie
 *
 * */
