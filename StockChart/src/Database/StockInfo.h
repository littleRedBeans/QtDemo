#ifndef STOCKINFO_H
#define STOCKINFO_H

#include <QString>

class StockInfo
{
public:
    StockInfo();
    StockInfo(const QString& tsCode, const QString& name);
    
    QString getTsCode() const;
    QString getName() const;
    
    void setTsCode(const QString& tsCode);
    void setName(const QString& name);
    
private:
    QString tsCode;  // 股票代码
    QString name;    // 股票名称
};

#endif // STOCKINFO_H
