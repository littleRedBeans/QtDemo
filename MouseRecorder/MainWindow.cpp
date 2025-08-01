#include "MainWindow.h"
#include <QMessageBox>
#include "ui_MainWindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mouseRecorder_.createActions(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox::information(nullptr, "", "btn1 clicked");
}

void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox::information(nullptr, "", "btn2 clicked");
}

void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox::information(nullptr, "", "btn3 clicked");
}
