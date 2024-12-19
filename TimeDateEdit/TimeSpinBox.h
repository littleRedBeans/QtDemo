#ifndef TIMESPINBOX_H
#define TIMESPINBOX_H

#include <QSpinBox>
class QKeyEvent;
class TimeSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit TimeSpinBox(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void pressLeft();
    void pressRight();
};

#endif // TIMESPINBOX_H
