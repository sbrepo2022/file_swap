#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint);//w.setWindowFlags(Qt::FramelessWindowHint|Qt::SplashScreen|Qt::WindowStaysOnBottomHint);

    QFile styleF;
    styleF.setFileName(":/style.css");
    styleF.open(QFile::ReadOnly);
    QString qssStr = styleF.readAll();
    w.setStyleSheet(qssStr);

    QSettings *settings;
    if (QFile::exists("settings.ini")) {
        settings = new QSettings("settings.ini", QSettings::IniFormat);
        settings->beginGroup("position");
        w.setGeometry(settings->value("x").toInt(), settings->value("y").toInt(), w.size().width(), w.size().height());
    }

    w.show();

    return a.exec();
}
