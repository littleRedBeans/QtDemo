#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlError>
#include <QThread>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class StockDataManager;
class QListWidgetItem;
class HTTPAccessManager;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_actionAddStock_triggered(bool checked);
    void on_stockListView_clicked(const QModelIndex &index);

private:
    void initDatabase();
    void initNetwork();
    void initStockListView();
    void addStockInfo();
    Ui::MainWindow *ui;
    StockDataManager *stockInfoDTM_;
    HTTPAccessManager *httpManager_;

    QThread databaseThread_;
    QThread networkThread_;
};
#endif // MAINWINDOW_H
