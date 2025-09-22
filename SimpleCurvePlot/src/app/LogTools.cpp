#include "LogTools.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/qt_sinks.h"

#include <QTextEdit>
void TL::initLogger(QTextEdit *edit)
{
    // 初始化异步日志线程池，队列大小8192，1个后台线程
    spdlog::init_thread_pool(8192, 1);

    // 创建循环文件sink (100MB * 3个文件)
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "log/app.log", 1024 * 1024 * 100, 3);

    // 设置日志格式
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    // 创建异步日志器
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(file_sink);

    auto logger = std::make_shared<spdlog::async_logger>(
                "TXT",
                sinks.begin(),
                sinks.end(),
                spdlog::thread_pool(),
                spdlog::async_overflow_policy::block);

    // 设置日志级别
    logger->set_level(spdlog::level::debug);
    spdlog::register_logger(logger);

    //创建ui日志器,最后一个参数默认设置为true使用utf8编码
    spdlog::qt_color_logger_mt("UI", edit, 1000,true)->set_level(spdlog::level::debug);
}

void TL::logText(QString text, Level level)
{
    switch (level) {
    case trace:
    {
        spdlog::get("UI")->trace(text.toStdString());
        spdlog::get("TXT")->trace(text.toStdString());
    }
        break;
    case debug:
    {
        spdlog::get("UI")->debug(text.toStdString());
        spdlog::get("TXT")->debug(text.toStdString());
    }
        break;
    case background:
    {
        spdlog::get("TXT")->info(text.toStdString());
    }
        break;
    case info:
    {
        spdlog::get("UI")->info(text.toStdString());
        spdlog::get("TXT")->info(text.toStdString());
    }
        break;
    case warn:
    {
        spdlog::get("UI")->warn(text.toStdString());
        spdlog::get("TXT")->warn(text.toStdString());
    }
        break;
    case err:
    {
        spdlog::get("UI")->error(text.toStdString());
        spdlog::get("TXT")->error(text.toStdString());
    }
        break;
    case critical:
    {
        spdlog::get("UI")->critical(text.toStdString());
        spdlog::get("TXT")->critical(text.toStdString());
    }
        break;
    default:
        break;
    }
}
