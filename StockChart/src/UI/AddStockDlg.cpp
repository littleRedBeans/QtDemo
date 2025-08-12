#include "AddStockDlg.h"
#include <QMessageBox>
#include "ui_AddStockDlg.h"
AddStockDlg::AddStockDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddStockDlg)
{
    ui->setupUi(this);
}

AddStockDlg::AddStockDlg(QString tsCode, QString name, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddStockDlg)
{
    ui->setupUi(this);
    ui->editStockCode->setText(tsCode);
    ui->editStockName->setText(name);
}

AddStockDlg::~AddStockDlg()
{
    delete ui;
}

QString AddStockDlg::getStockName() const
{
    return ui->editStockName->text().trimmed();
}

QString AddStockDlg::getStockCode() const
{
    return ui->editStockCode->text().trimmed();
}

void AddStockDlg::setStockName(QString tsCode)
{
    ui->editStockName->setText(tsCode);
}

void AddStockDlg::setStockCode(QString name)
{
    ui->editStockCode->setText(name);
}

void AddStockDlg::on_buttonBox_accepted()
{
    if (getStockCode().isEmpty() || getStockName().isEmpty()) {
        QMessageBox::warning(this, "警告", "股票代码和名称不能为空！");
        return;
    }
}
