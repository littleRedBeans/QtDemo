#include "TimeDateEdit.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TimeDateEdit w;
    w.setWindowTitle("时间设置");
    w.show();
    return a.exec();
}
