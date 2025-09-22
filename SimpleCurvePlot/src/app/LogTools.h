#ifndef LOGTOOLS_H
#define LOGTOOLS_H
#include <QString>
class QTextEdit;
class TL
{
public:
    enum Level{
        trace = 0,
        debug,
        background,
        info,
        warn,
        err,
        critical,
    };
    static void initLogger(QTextEdit* edit);
    static void logText(QString text,Level level = Level::info);
};


#endif // LOGTOOLS_H
