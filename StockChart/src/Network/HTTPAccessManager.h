#ifndef HTTPACCESSMANAGER_H
#define HTTPACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include "Database/StockData.h"
class HTTPAccessManager : public QObject
{
    Q_OBJECT
public:
    explicit HTTPAccessManager(QObject *parent = nullptr);
    // 从API获取股票数据
    void fetchStockDataFromApi(const QString &tsCode, const QDate &startDate, const QDate &endDate);
public slots:
    void onFetchStockDataFromApi(const QString &tsCode,
                                 const QDate &startDate,
                                 const QDate &endDate);
signals:
    void sigStockDataList(QList<StockData> dataList);
private slots:
    // 处理网络响应
    void onNetworkReplyFinished(QNetworkReply *reply);

private:
    // 解析JSON响应
    QList<StockData> parseJsonResponse(const QByteArray &jsonData);
    QNetworkAccessManager *networkManager;
    QString apiToken;
};

#endif // HTTPACCESSMANAGER_H
