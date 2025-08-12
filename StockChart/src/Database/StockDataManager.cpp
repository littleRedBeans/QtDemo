#include "StockDataManager.h"
#include <QDateTime>
#include <QDebug>
#include <QSqlQuery>
#include <QString>
#include <QThread>
#include "qsqlerror.h"

const QString STOCK_DATABASE = QLatin1String("STOCK_DATABSE");
StockDataManager::StockDataManager(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<StockInfo>("StockInfo");
    qRegisterMetaType<QList<StockInfo>>("QList<StockInfo>");
    qRegisterMetaType<QList<StockData>>("QList<StockData>");
}

StockDataManager::~StockDataManager() {}

void StockDataManager::addStock(const StockInfo &stock)
{
    QMetaObject::invokeMethod(this, "onAddStock", Qt::AutoConnection, Q_ARG(StockInfo, stock));
}

void StockDataManager::removeStock(const QString &tsCode)
{
    QMetaObject::invokeMethod(this, "onRemoveStock", Qt::AutoConnection, Q_ARG(QString, tsCode));
}

void StockDataManager::initDatabase()
{
    // 打开SQLite数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",
                                                STOCK_DATABASE
                                                    + QString("%1").arg(
                                                        (quintptr) QThread::currentThread()));
    db.setDatabaseName("stock_data.db");
    if (!db.open()) {
        emit sigDatabaseError(db.lastError());
    }

    createTables();
}

void StockDataManager::createTables()
{
    QSqlQuery query(QSqlDatabase::database(
        STOCK_DATABASE + QString("%1").arg((quintptr) QThread::currentThread())));

    // 创建股票信息表
    QString stockTableSql = QLatin1String("CREATE TABLE IF NOT EXISTS stocks ("
                                          "ts_code TEXT PRIMARY KEY,"
                                          "name TEXT NOT NULL)");
    if (!query.exec(stockTableSql)) {
        emit sigDatabaseError(query.lastError());
    }
    
    // 创建股票数据表
    QString dataTableSql = QLatin1String("CREATE TABLE IF NOT EXISTS stock_data ("
                                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                         "ts_code TEXT NOT NULL,"
                                         "trade_date DATE NOT NULL,"
                                         "adjustment_type TEXT NOT NULL,"
                                         "open REAL,"
                                         "high REAL,"
                                         "low REAL,"
                                         "close REAL,"
                                         "pre_close REAL,"
                                         "change REAL,"
                                         "pct_chg REAL,"
                                         "vol REAL,"
                                         "amount REAL,"
                                         "UNIQUE(ts_code, trade_date, adjustment_type)"
                                         ")");
    if (!query.exec(dataTableSql)) {
        emit sigDatabaseError(query.lastError());
    }
}

void StockDataManager::getAllStocks()
{
    QList<StockInfo> stocks;
    QSqlQuery query(QLatin1String("SELECT ts_code, name FROM stocks ORDER BY ts_code"),
                    QSqlDatabase::database(
                        STOCK_DATABASE + QString("%1").arg((quintptr) QThread::currentThread())));

    while (query.next()) {
        QString tsCode = query.value(0).toString();
        QString name = query.value(1).toString();
        stocks.append(StockInfo(tsCode, name));
    }
    emit sigStockInfos(stocks);
}

void StockDataManager::onAddStock(const StockInfo &stock)
{
    QSqlQuery query(QSqlDatabase::database(
        STOCK_DATABASE + QString("%1").arg((quintptr) QThread::currentThread())));
    if (stockExists(stock.getTsCode())) {
        query.prepare("UPDATE stocks SET name = :name WHERE ts_code = :ts_code");
        query.bindValue(":name", stock.getName());
        query.bindValue(":ts_code", stock.getTsCode());
    } else {
        query.prepare("INSERT INTO stocks (ts_code, name) VALUES (:ts_code, :name)");
        query.bindValue(":ts_code", stock.getTsCode());
        query.bindValue(":name", stock.getName());
    }

    if (!query.exec()) {
        emit sigDatabaseError(query.lastError());
    }
}

void StockDataManager::onRemoveStock(const QString &tsCode)
{
    QSqlQuery query(QSqlDatabase::database(
        STOCK_DATABASE + QString("%1").arg((quintptr) QThread::currentThread())));

    // 先删除相关数据
    query.prepare("DELETE FROM stock_data WHERE ts_code = :ts_code");
    query.bindValue(":ts_code", tsCode);
    if (!query.exec()) {
        emit sigDatabaseError(query.lastError());
    }

    // 再删除股票信息
    query.prepare("DELETE FROM stocks WHERE ts_code = :ts_code");
    query.bindValue(":ts_code", tsCode);
    if (!query.exec()) {
        emit sigDatabaseError(query.lastError());
    }
}

bool StockDataManager::stockExists(const QString& tsCode)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM stocks WHERE ts_code = :ts_code");
    query.bindValue(":ts_code", tsCode);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

void StockDataManager::getStockDataFromDb(const QString &tsCode)
{
    QMetaObject::invokeMethod(this,
                              "onGetStockDataFromDb",
                              Qt::AutoConnection,
                              Q_ARG(const QString &, tsCode));
}

void StockDataManager::saveStockDataToDb(QList<StockData> dataList)
{
    qDebug() << "saveStockDataToDb start:";
    if (dataList.isEmpty()) {
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(
        STOCK_DATABASE + QString("%1").arg((quintptr) QThread::currentThread()));

    QSqlQuery query(db);
    db.transaction(); // 开始事务，提高插入效率

    // 先删除该股票的所有旧数据
    query.prepare("DELETE FROM stock_data WHERE ts_code = :ts_code");
    query.bindValue(":ts_code", dataList.first().getTsCode());
    if (!query.exec()) {
        emit sigDatabaseError(query.lastError());
        db.rollback();
        return;
    }
    
    // 插入新数据
    query.prepare(
        "INSERT INTO stock_data (ts_code, trade_date, adjustment_type, open, high, low, close, "
        "pre_close, change, pct_chg, vol, amount) "
        "VALUES (:ts_code, :trade_date, :adjustment_type, :open, :high, :low, :close, "
        ":pre_close, :change, :pct_chg, :vol, :amount)");

    foreach (const StockData& data, dataList) {
        query.bindValue(":ts_code", data.getTsCode());
        query.bindValue(":trade_date", data.getTradeDate());
        query.bindValue(":adjustment_type", data.getAdjustmentType());
        query.bindValue(":open", data.getOpen());
        query.bindValue(":high", data.getHigh());
        query.bindValue(":low", data.getLow());
        query.bindValue(":close", data.getClose());
        query.bindValue(":pre_close", data.getPreClose());
        query.bindValue(":change", data.getChange());
        query.bindValue(":pct_chg", data.getPctChg());
        query.bindValue(":vol", data.getVol());
        query.bindValue(":amount", data.getAmount());
        
        if (!query.exec()) {
            emit sigDatabaseError(query.lastError());
            db.rollback();
            return;
        }
    }
    
    db.commit();  // 提交事务
    qDebug() << "saveStockDataToDb finished:";
}

void StockDataManager::onGetStockDataFromDb(const QString &tsCode)
{
    QList<StockData> result;

    QSqlQuery query(QSqlDatabase::database(
        STOCK_DATABASE + QString("%1").arg((quintptr) QThread::currentThread())));
    query.prepare("SELECT trade_date, open, high, low, close, pre_close, "
                  "change, pct_chg, vol, amount FROM stock_data "
                  "WHERE ts_code = :ts_code ORDER BY trade_date");
    query.bindValue(":ts_code", tsCode);

    if (!query.exec()) {
        emit sigDatabaseError(query.lastError());
    }

    while (query.next()) {
        StockData data;
        data.setTsCode(tsCode);
        data.setTradeDate(query.value(0).toDate());
        data.setOpen(query.value(1).toDouble());
        data.setHigh(query.value(2).toDouble());
        data.setLow(query.value(3).toDouble());
        data.setClose(query.value(4).toDouble());
        data.setPreClose(query.value(5).toDouble());
        data.setChange(query.value(6).toDouble());
        data.setPctChg(query.value(7).toDouble());
        data.setVol(query.value(8).toDouble());
        data.setAmount(query.value(9).toDouble());

        result.append(data);
    }

    emit sigStockDataList(tsCode, result);
}
