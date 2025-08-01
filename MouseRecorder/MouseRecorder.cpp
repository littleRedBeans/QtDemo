#include "MouseRecorder.h"
#include <QApplication>
#include <QDateTime>
#include <QEvent>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QTest>
#include <QTimer>
#include <QWindow>
GlobalEventFilter g_globalEventFilter;
MouseRecorder::MouseRecorder(QObject *parent)
    : QObject{parent}
    , playTimer(new QTimer(this))
{
    g_globalEventFilter.mouseRecorder_ = this;
    connect(playTimer, &QTimer::timeout, this, &MouseRecorder::playNextAction);
}

void MouseRecorder::createActions(QMainWindow *mainwindow)
{
    // 创建上下文菜单
    QMenu *contextMenu = new QMenu(tr("鼠标录制"), mainwindow->centralWidget());

    // 添加菜单项
    contextMenu->addAction("开始", this, [this]() {
        actions.clear();
        qApp->installEventFilter(&g_globalEventFilter);
    });
    contextMenu->addAction("回放", this, [this]() {
        currentActionIndex = 0;
        playTimer->start(0);
    });
    contextMenu->addAction("停止", this, [this]() {
        playTimer->stop();
        qApp->removeEventFilter(&g_globalEventFilter);
    });
    contextMenu->addAction("保存", this, [this]() {
        if (actions.isEmpty())
            return;

        QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                        "保存录制",
                                                        "",
                                                        "鼠标录制文件 (*.mrec)");
        if (fileName.isEmpty())
            return;

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "# 鼠标录制文件\n";
            out << "# 时间戳(ms),X坐标,Y坐标,按钮,按下/释放\n";

            for (const auto &action : actions) {
                out << action.timestamp << "," << action.x << "," << action.y << ","
                    << static_cast<int>(action.button) << "," << (action.isPress ? "1" : "0")
                    << "\n";
            }
            file.close();
        }
    });
    contextMenu->addAction("加载", this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                        "加载录制",
                                                        "",
                                                        "鼠标录制文件 (*.mrec)");
        if (fileName.isEmpty())
            return;

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            actions.clear();
            QTextStream in(&file);

            // 跳过标题行
            in.readLine();
            in.readLine();

            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.isEmpty())
                    continue;

                QStringList parts = line.split(',');
                if (parts.size() >= 5) {
                    MouseAction action;
                    action.timestamp = parts[0].toLongLong();
                    action.x = parts[1].toInt();
                    action.y = parts[2].toInt();
                    action.button = static_cast<Qt::MouseButton>(parts[3].toInt());
                    action.isPress = (parts[4].toInt() == 1);
                    actions.append(action);
                }
            }

            file.close();
        }
    });

    // 为中央部件设置右键菜单策略：使用自定义菜单
    mainwindow->centralWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
    // 连接右键菜单触发信号
    connect(mainwindow->centralWidget(),
            &QWidget::customContextMenuRequested,
            this,
            [contextMenu, mainwindow](const QPoint &pos) {
                // 显示菜单（pos 是相对于控件的坐标，需转换为全局坐标）
                contextMenu->exec(mainwindow->centralWidget()->mapToGlobal(pos));
            });
}

void MouseRecorder::mousePress(QMouseEvent *event)
{
    MouseAction action;
    static qint64 prePressTime = 0;
    action.timestamp = QDateTime::currentMSecsSinceEpoch() - startTime;
    if (action.timestamp - prePressTime < 500) {
        return;
    }
    prePressTime = action.timestamp;
    action.x = event->globalPosition().toPoint().x();
    action.y = event->globalPosition().toPoint().y();
    action.button = event->button();
    if (Qt::LeftButton == action.button) {
        action.isPress = true;
        actions.append(action);
    }
}

void MouseRecorder::mouseMove(QMouseEvent *event)
{
    // 仅在拖动时记录移动（减少数据量）
    if (event->buttons() == Qt::NoButton) {
        static QPoint lastPos;
        QPoint currentPos = event->globalPosition().toPoint();

        // 限制记录频率，避免过多数据点
        if ((currentPos - lastPos).manhattanLength() > 5) {
            MouseAction action;
            action.timestamp = QDateTime::currentMSecsSinceEpoch() - startTime;
            action.x = currentPos.x();
            action.y = currentPos.y();
            action.button = Qt::NoButton;
            action.isPress = false;
            actions.append(action);
            lastPos = currentPos;
        }
    }
}

void MouseRecorder::playNextAction()
{
    //循环播放
    if (currentActionIndex >= actions.size()) {
        currentActionIndex = 0;
    }

    QWindow *targetWindow = findTargetWindow();
    if (!targetWindow) {
        playTimer->stop();
        return;
    }

    const MouseAction &action = actions[currentActionIndex];
    // 使用QTimer::singleShot确保事件循环继续运行
    QTimer::singleShot(0, [this, action, targetWindow]() {
        // 移动鼠标
        QCursor::setPos(action.x, action.y);

        // 模拟按键事件
        if (action.button != Qt::NoButton) {
            if (action.isPress) {
                QTest::mouseClick(targetWindow,
                                  action.button,
                                  Qt::NoModifier,
                                  QPoint(action.x, action.y));
            } else {
                QTest::mouseRelease(targetWindow,
                                    action.button,
                                    Qt::NoModifier,
                                    QPoint(action.x, action.y));
            }
        }

        // 更新状态
        currentActionIndex++;
        // 安排下一个动作
        if (currentActionIndex < actions.size()) {
            qint64 delay = actions[currentActionIndex].timestamp
                           - actions[currentActionIndex - 1].timestamp;
            if (delay < 0)
                delay = 0;
            playTimer->start(delay);
        }
    });
}

QWindow *MouseRecorder::findTargetWindow() const
{
#if (QT_VERSION <= QT_VERSION_CHECK(6, 0, 0))
    // 尝试找到当前活动窗口
    QWindow *activeWindow = QGuiApplication::activeWindow();
    if (activeWindow)
        return activeWindow;

    // 如果没有活动窗口，尝试使用第一个可见窗口
    for (QWindow *window : QGuiApplication::topLevelWindows()) {
        if (window->isVisible())
            return window;
    }
#else
    // 获取当前活动的QWidget
    QWidget *activeWidget = QApplication::activeWindow();
    if (activeWidget) {
        // 将QWidget转换为QWindow
        QWindow *window = activeWidget->windowHandle();
        if (window)
            return window;
    }

    // 如果没有活动窗口，尝试使用第一个可见窗口
    for (QWindow *window : QGuiApplication::topLevelWindows()) {
        if (window->isVisible())
            return window;
    }
#endif
    return nullptr;
}

bool GlobalEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        mouseRecorder_->mousePress(static_cast<QMouseEvent *>(event));
    } else if (event->type() == QEvent::MouseMove) {
        mouseRecorder_->mouseMove(static_cast<QMouseEvent *>(event));
    } else {
        ; /*no deal with*/
    }
    return QObject::eventFilter(watched, event);
}
