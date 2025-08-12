#include "HTTPAccessManager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
HTTPAccessManager::HTTPAccessManager(QObject *parent)
    : QObject{parent}
    , networkManager(new QNetworkAccessManager(this))
    , apiToken("49c631148cd34b8ab78f0052a41e9960a0fb04e52424f632e980e621")
{
    connect(networkManager,
            &QNetworkAccessManager::finished,
            this,
            &HTTPAccessManager::onNetworkReplyFinished);
}
void HTTPAccessManager::fetchStockDataFromApi(const QString &tsCode,
                                              const QDate &startDate,
                                              const QDate &endDate)
{
    QMetaObject::invokeMethod(this,
                              "onFetchStockDataFromApi",
                              Qt::AutoConnection,
                              Q_ARG(const QString &, tsCode),
                              Q_ARG(const QDate &, startDate),
                              Q_ARG(const QDate &, endDate));
}

void HTTPAccessManager::onFetchStockDataFromApi(const QString &tsCode,
                                                const QDate &startDate,
                                                const QDate &endDate)
{
    // 构建API请求
    QUrl url("http://api.tushare.pro");
    QJsonObject params;
    params["ts_code"] = tsCode;
    params["start_date"] = startDate.toString("yyyyMMdd");
    params["end_date"] = endDate.toString("yyyyMMdd");

    QJsonObject requestData;
    requestData["api_name"] = "daily";
    requestData["token"] = apiToken;
    requestData["params"] = params;
    requestData["fields"]
        = "ts_code,trade_date,open,high,low,close,pre_close,change,pct_chg,vol,amount";

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkRequest request(url);
    //置Content-Type头部字段，明确告诉服务器：客户端发送的请求体（body）数据格式为 JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    networkManager->post(request, data);
}
void HTTPAccessManager::onNetworkReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "网络请求错误：" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    // 解析响应并保存到数据库
    QList<StockData> stockDataList = parseJsonResponse(responseData);

    if (!stockDataList.isEmpty()) {
        emit sigStockDataList(stockDataList);
    }
}

QList<StockData> HTTPAccessManager::parseJsonResponse(const QByteArray &jsonData)
{
    QList<StockData> result;

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qDebug() << "解析JSON失败";
        return result;
    }

    QJsonObject root = doc.object();
    if (root["code"].toInt() != 0) {
        qDebug() << "API错误：" << root["msg"].toString();
        return result;
    }

    QJsonArray dataArray = root["data"].toObject()["items"].toArray();
    for (int i = 0; i < dataArray.size(); ++i) {
        QJsonArray item = dataArray[i].toArray();

        StockData data;
        data.setTsCode(item[0].toString());
        data.setTradeDate(QDate::fromString(item[1].toString(), "yyyyMMdd"));
        data.setAdjustmentType("Forward");
        data.setOpen(item[2].toDouble());
        data.setHigh(item[3].toDouble());
        data.setLow(item[4].toDouble());
        data.setClose(item[5].toDouble());
        data.setPreClose(item[6].toDouble());
        data.setChange(item[7].toDouble());
        data.setPctChg(item[8].toDouble());
        data.setVol(item[9].toDouble());
        data.setAmount(item[10].toDouble());

        result.append(data);
    }

    return result;
}
