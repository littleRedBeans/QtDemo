#ifndef ADDSTOCKDLG_H
#define ADDSTOCKDLG_H

#include <QDialog>

namespace Ui {
class AddStockDlg;
}

class AddStockDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddStockDlg(QWidget *parent = nullptr);
    AddStockDlg(QString tsCode, QString name, QWidget *parent = nullptr);
    ~AddStockDlg();
    QString getStockName() const;
    QString getStockCode() const;
    void setStockName(QString tsCode);
    void setStockCode(QString name);
private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddStockDlg *ui;
};

#endif // ADDSTOCKDLG_H
