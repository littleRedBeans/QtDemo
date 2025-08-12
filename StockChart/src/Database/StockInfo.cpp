#include "StockInfo.h"

StockInfo::StockInfo()
{
}

StockInfo::StockInfo(const QString& tsCode, const QString& name)
    : tsCode(tsCode), name(name)
{
}

QString StockInfo::getTsCode() const
{
    return tsCode;
}

QString StockInfo::getName() const
{
    return name;
}

void StockInfo::setTsCode(const QString& tsCode)
{
    this->tsCode = tsCode;
}

void StockInfo::setName(const QString& name)
{
    this->name = name;
}
