#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadPageData();
    void applyFilter();
    void onPrevPage();
    void onNextPage();
    void goToPage();
    void changePageSize();

private:
    bool initDatabase();
    int getTotalRowCount();
    void updatePageInfo();

    QSqlQueryModel *m_model;
    QTableView *m_tableView;
    QLineEdit *m_filterEdit;
    QLineEdit *m_pageEdit;
    QLineEdit *m_pageSizeEdit;
    QLabel *m_pageInfoLabel;
    QLabel *m_statusLabel;

    int m_currentPage;
    int m_pageSize;
    QString m_currentFilter;
    QString m_tableName;
    int m_totalRows;
};
#endif // MAINWINDOW_H
