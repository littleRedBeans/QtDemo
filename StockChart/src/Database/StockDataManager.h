#ifndef STOCKDATAMANAGER_H
#define STOCKDATAMANAGER_H

#include <QDate>
#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include "StockData.h"
#include "StockInfo.h"

class StockDataManager : public QObject
{
    Q_OBJECT
public:
    explicit StockDataManager(QObject *parent = nullptr);
    ~StockDataManager();

    void addStock(const StockInfo &stock);

    void removeStock(const QString &tsCode);

    // 检查股票是否已存在
    bool stockExists(const QString &tsCode);

    // 从数据库获取股票数据
    void getStockDataFromDb(const QString &tsCode);

public slots:
    // 初始化数据库
    void initDatabase();
    // 获取所有股票信息
    void getAllStocks();
    // 添加股票
    void onAddStock(const StockInfo &stock);
    // 删除股票
    void onRemoveStock(const QString &tsCode);
    // 保存股票数据到数据库
    void saveStockDataToDb(QList<StockData> dataList);
    void onGetStockDataFromDb(const QString &tsCode);
signals:
    // 数据获取完成信号
    void stockDataFetched(const QString &tsCode, bool success);
    void sigDatabaseError(QSqlError error);
    void sigStockInfos(QList<StockInfo> stocks);
    void sigStockDataList(QString tsCode, QList<StockData> dataList);

private:
    // 创建数据库表
    void createTables();
};

#endif // STOCKDATAMANAGER_H
