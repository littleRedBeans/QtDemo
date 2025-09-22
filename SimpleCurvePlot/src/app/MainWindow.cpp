#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "LogTools.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //创建日志栏
    createLogWdget();

    //添加tab页
    initTabWidget();
    TL::logText("你好");
    TL::logText("我好");
    TL::logText("你好",TL::warn);
    TL::logText("我好",TL::err);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createLogWdget()
{
    TL::initLogger(&editLog_);
    editLog_.setReadOnly(true);
}

void MainWindow::initTabWidget()
{
    ui->tabWidget->addTab(&editLog_,"日志");
}

