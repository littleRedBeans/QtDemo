#include "TimeDateEdit.h"
#include <QDateTime>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include "./ui_TimeDateEdit.h"
#include "TimeSpinBox.h"
TimeDateEdit::TimeDateEdit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimeDateEdit)
{
    ui->setupUi(this);
    QHBoxLayout *layout = new QHBoxLayout(this);

    QStringList names{"年", "月", "日", "时", "分", "秒"};
    for (QString name : names) {
        timeSpinBoxs_.emplace_back(new TimeSpinBox(this));
        layout->addWidget(timeSpinBoxs_.back());
        timeSpinBoxs_.back()->setObjectName(name);
        layout->addWidget(new QLabel(name, this));
        connect(timeSpinBoxs_.back(), &TimeSpinBox::pressLeft, this, &TimeDateEdit::onLeftPress);
        connect(timeSpinBoxs_.back(), &TimeSpinBox::pressRight, this, &TimeDateEdit::onRightPress);
    }
    timeSpinBoxs_[year]->setRange(2000, 3000);
    timeSpinBoxs_[month]->setRange(1, 12);
    timeSpinBoxs_[day]->setRange(1, 31);
    timeSpinBoxs_[hour]->setRange(0, 23);
    timeSpinBoxs_[minute]->setRange(0, 59);
    timeSpinBoxs_[second]->setRange(0, 59);
}

TimeDateEdit::~TimeDateEdit()
{
    delete ui;
}

void TimeDateEdit::onLeftPress()
{
    if ("年" == sender()->objectName()) {
        timeSpinBoxs_[second]->setFocus();
    } else if ("月" == sender()->objectName()) {
        timeSpinBoxs_[year]->setFocus();
    } else if ("日" == sender()->objectName()) {
        timeSpinBoxs_[month]->setFocus();
    } else if ("时" == sender()->objectName()) {
        timeSpinBoxs_[day]->setFocus();
    } else if ("分" == sender()->objectName()) {
        timeSpinBoxs_[hour]->setFocus();
    } else if ("秒" == sender()->objectName()) {
        timeSpinBoxs_[minute]->setFocus();
    }
}

void TimeDateEdit::onRightPress()
{
    if ("年" == sender()->objectName()) {
        timeSpinBoxs_[month]->setFocus();
    } else if ("月" == sender()->objectName()) {
        timeSpinBoxs_[day]->setFocus();
    } else if ("日" == sender()->objectName()) {
        timeSpinBoxs_[hour]->setFocus();
    } else if ("时" == sender()->objectName()) {
        timeSpinBoxs_[minute]->setFocus();
    } else if ("分" == sender()->objectName()) {
        timeSpinBoxs_[second]->setFocus();
    } else if ("秒" == sender()->objectName()) {
        timeSpinBoxs_[year]->setFocus();
    }
}

void TimeDateEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        bool res = QDateTime(QDate(timeSpinBoxs_[year]->value(),
                                   timeSpinBoxs_[month]->value(),
                                   timeSpinBoxs_[day]->value()),
                             QTime(timeSpinBoxs_[hour]->value(),
                                   timeSpinBoxs_[minute]->value(),
                                   timeSpinBoxs_[second]->value()))
                       .isValid();
        if (res) {
            ; //设置系统时间
            close();
        } else {
            QMessageBox::warning(nullptr, "", "输入时间不合法");
            event->accept();
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}
