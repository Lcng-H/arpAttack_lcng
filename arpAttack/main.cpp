#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //设置全局字体
    QFont font  = a.font();
    font.setPointSize(15);
    a.setFont(font);

    MainWindow w;
    w.show();
    return a.exec();
}
