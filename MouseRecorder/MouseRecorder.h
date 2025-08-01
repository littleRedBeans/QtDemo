#ifndef MOUSERECORDER_H
#define MOUSERECORDER_H

#include <QObject>
class QTimer;
class QMainWindow;
class QMouseEvent;
class QWindow;

class MouseRecorder : public QObject
{
    Q_OBJECT
    struct MouseAction
    {
        qint64 timestamp;
        int x;
        int y;
        Qt::MouseButton button;
        bool isPress;
    };

public:
    explicit MouseRecorder(QObject *parent = nullptr);
    void createActions(QMainWindow *mainwindow);
    void mousePress(QMouseEvent *event);
    void mouseMove(QMouseEvent *event);
private slots:
    void playNextAction();

private:
    QWindow *findTargetWindow() const;
    QTimer *playTimer;
    qint64 startTime{0};
    int currentActionIndex{0};
    QList<MouseAction> actions;
};

class GlobalEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit GlobalEventFilter(QObject *parent = nullptr)
        : QObject(parent)
    {}

    bool eventFilter(QObject *watched, QEvent *event) override;

public:
    MouseRecorder *mouseRecorder_{nullptr};
};
#endif // MOUSERECORDER_H
