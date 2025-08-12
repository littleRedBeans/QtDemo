#include "StockData.h"

StockData::StockData()
    : open(0), high(0), low(0), close(0), preClose(0),
      change(0), pctChg(0), vol(0), amount(0)
{
}

QString StockData::getTsCode() const
{
    return tsCode;
}

QDate StockData::getTradeDate() const
{
    return tradeDate;
}

QString StockData::getAdjustmentType() const
{
    return adjustmentType;
}

double StockData::getOpen() const
{
    return open;
}

double StockData::getHigh() const
{
    return high;
}

double StockData::getLow() const
{
    return low;
}

double StockData::getClose() const
{
    return close;
}

double StockData::getPreClose() const
{
    return preClose;
}

double StockData::getChange() const
{
    return change;
}

double StockData::getPctChg() const
{
    return pctChg;
}

double StockData::getVol() const
{
    return vol;
}

double StockData::getAmount() const
{
    return amount;
}

void StockData::setTsCode(const QString& tsCode)
{
    this->tsCode = tsCode;
}

void StockData::setTradeDate(const QDate& tradeDate)
{
    this->tradeDate = tradeDate;
}

void StockData::setAdjustmentType(const QString &type)
{
    adjustmentType = type;
}

void StockData::setOpen(double open)
{
    this->open = open;
}

void StockData::setHigh(double high)
{
    this->high = high;
}

void StockData::setLow(double low)
{
    this->low = low;
}

void StockData::setClose(double close)
{
    this->close = close;
}

void StockData::setPreClose(double preClose)
{
    this->preClose = preClose;
}

void StockData::setChange(double change)
{
    this->change = change;
}

void StockData::setPctChg(double pctChg)
{
    this->pctChg = pctChg;
}

void StockData::setVol(double vol)
{
    this->vol = vol;
}

void StockData::setAmount(double amount)
{
    this->amount = amount;
}
