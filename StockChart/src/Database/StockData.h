#ifndef STOCKDATA_H
#define STOCKDATA_H

#include <QString>
#include <QDate>

class StockData
{
public:
    StockData();
    
    QString getTsCode() const;
    QDate getTradeDate() const;
    QString getAdjustmentType() const;
    double getOpen() const;
    double getHigh() const;
    double getLow() const;
    double getClose() const;
    double getPreClose() const;
    double getChange() const;
    double getPctChg() const;
    double getVol() const;
    double getAmount() const;
    
    void setTsCode(const QString& tsCode);
    void setTradeDate(const QDate& tradeDate);
    void setAdjustmentType(const QString &type);
    void setOpen(double open);
    void setHigh(double high);
    void setLow(double low);
    void setClose(double close);
    void setPreClose(double preClose);
    void setChange(double change);
    void setPctChg(double pctChg);
    void setVol(double vol);
    void setAmount(double amount);
    
private:
    QString tsCode;      // 股票代码
    QDate tradeDate;     // 交易日期
    QString adjustmentType; //复权状态 Forward前复权 Backward后复权
    double open;         // 开盘价
    double high;         // 最高价
    double low;          // 最低价
    double close;        // 收盘价
    double preClose;     // 昨收价
    double change;       // 涨跌额
    double pctChg;       // 涨跌幅
    double vol;          // 成交量
    double amount;       // 成交额
};

#endif // STOCKDATA_H
