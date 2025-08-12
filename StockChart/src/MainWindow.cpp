#include "MainWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QThread>
#include <QTimer>
#include "Database/StockDataManager.h"
#include "Network/HTTPAccessManager.h"
#include "UI/AddStockDlg.h"
#include "ui_MainWindow.h"
QSqlTableModel *g_StockListmodel = nullptr;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initNetwork();
    initDatabase();
    initStockListView();
}

MainWindow::~MainWindow()
{
    databaseThread_.quit();
    databaseThread_.wait();

    networkThread_.quit();
    networkThread_.wait();
    delete ui;
}

void MainWindow::on_actionAddStock_triggered(bool checked)
{
    Q_UNUSED(checked)
    addStockInfo();
}

void MainWindow::addStockInfo()
{
    AddStockDlg dlg;
    int ret = dlg.exec();
    if (ret == QDialog::Rejected) {
        return;
    }
    StockInfo info(dlg.getStockCode(), dlg.getStockName());
    QSqlRecord record = g_StockListmodel->record();
    record.setValue("ts_code", dlg.getStockCode());
    record.setValue("name", dlg.getStockName());
    g_StockListmodel->insertRecord(-1, record);
    g_StockListmodel->submitAll(); // 提交更改到数据库
}

void MainWindow::initDatabase()
{
    databaseThread_.setObjectName(tr("Database"));
    stockInfoDTM_ = new StockDataManager;
    stockInfoDTM_->moveToThread(&databaseThread_);
    connect(stockInfoDTM_, &StockDataManager::sigDatabaseError, this, [this](QSqlError err) {
        QMessageBox::critical(this, "Error Database", "Error Database: " + err.text());
    });

    connect(stockInfoDTM_,
            &StockDataManager::sigStockDataList,
            this,
            [this](QString tsCode, QList<StockData> dataList) {
                if (dataList.isEmpty()) {
                    QDate endDate = QDate::currentDate();
                    QDate startDate = endDate.addYears(-10);
                    httpManager_->fetchStockDataFromApi(tsCode, startDate, endDate);
                } else {
                    qDebug() << "get data from DB:";
                }
            });

    connect(httpManager_,
            &HTTPAccessManager::sigStockDataList,
            stockInfoDTM_,
            &StockDataManager::saveStockDataToDb);
    databaseThread_.start();
    QTimer::singleShot(0, stockInfoDTM_, &StockDataManager::initDatabase);
}

void MainWindow::initNetwork()
{
    networkThread_.setObjectName(tr("network"));
    httpManager_ = new HTTPAccessManager;
    httpManager_->moveToThread(&networkThread_);
    networkThread_.start();
}

void MainWindow::initStockListView()
{
    QSqlDatabase db
        = QSqlDatabase::addDatabase("QSQLITE",
                                    tr("STOCK_DATABSE%1").arg((quintptr) QThread::currentThread()));
    db.setDatabaseName("stock_data.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Error Database", "Error Database: " + db.lastError().text());
        return;
    }
    // 创建模型并关联到数据表
    g_StockListmodel = new QSqlTableModel(ui->stockListView, db);
    g_StockListmodel->setTable("stocks");                             // 设置要关联的数据表
    g_StockListmodel->setEditStrategy(QSqlTableModel::OnFieldChange); // 字段改变时立即更新数据库
    g_StockListmodel->select();
    ui->stockListView->setModel(g_StockListmodel); // 将模型设置到视图
    ui->stockListView->setModelColumn(1);          // 指定显示哪一列（可选）

    ui->stockListView->setAlternatingRowColors(true);
    QAction *delAction = new QAction(style()->standardIcon(QStyle::SP_DialogDiscardButton),
                                     "删除",
                                     ui->stockListView);
    ui->stockListView->addAction(delAction);
    connect(delAction, &QAction::triggered, this, [=]() {
        if (QMessageBox::Yes == QMessageBox::question(this, "删除股票", "是否删除？")) {
            QModelIndex index = ui->stockListView->currentIndex();
            if (index.isValid()) {
                g_StockListmodel->removeRow(index.row());
                g_StockListmodel->submitAll();
            }
        }
    });
    ui->stockListView->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MainWindow::on_stockListView_clicked(const QModelIndex &index)
{
    QString code = g_StockListmodel->record(index.row()).value("ts_code").toString();
    stockInfoDTM_->getStockDataFromDb(code);
}
