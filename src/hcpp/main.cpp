/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#include <QtWidgets>
#include <QSplashScreen>
#include <QTimer>
#include "samegame.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/resources/img/logo"));
    splash->show();
    SameGame w;
    QTimer::singleShot(2000, splash, SLOT(close()));
    QTimer::singleShot(2000, &w, SLOT(show()));

    // qSameGame w;
    // QTimer::singleShot(0, &w, SLOT(show()));

    return a.exec();
}
