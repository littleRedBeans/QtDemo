#include "TimeSpinBox.h"
#include <QKeyEvent>
TimeSpinBox::TimeSpinBox(QWidget *parent)
    : QSpinBox{parent}
{
    
}

void TimeSpinBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {
        emit pressLeft();
    } else if (event->key() == Qt::Key_Right) {
        emit pressRight();
    } else {
        QSpinBox::keyPressEvent(event);
    }
}
