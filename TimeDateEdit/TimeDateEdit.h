#ifndef TIMEDATEEDIT_H
#define TIMEDATEEDIT_H

#include <QWidget>
#include <vector>
class TimeSpinBox;
class QKeyEvent;
QT_BEGIN_NAMESPACE
namespace Ui { class TimeDateEdit; }
QT_END_NAMESPACE

class TimeDateEdit : public QWidget
{
    Q_OBJECT

public:
    enum DateType : quint32 { year = 0, month, day, hour, minute, second };
    TimeDateEdit(QWidget *parent = nullptr);
    ~TimeDateEdit();
private slots:
    void onLeftPress();
    void onRightPress();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::TimeDateEdit *ui;
    std::vector<TimeSpinBox *> timeSpinBoxs_;
};
#endif // TIMEDATEEDIT_H
