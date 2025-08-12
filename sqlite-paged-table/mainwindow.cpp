#include "mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentPage(1)
    , m_pageSize(20)
    , m_currentFilter("")
    , m_tableName("employee")
    , m_totalRows(0)
{
    if (!initDatabase()) {
        QMessageBox::critical(this, "数据库错误", "无法连接到数据库！");
        close();
        return;
    }

    // 初始化模型
    m_model = new QSqlQueryModel(this);

    // 初始化表格视图
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 过滤控件
    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText("示例: department='技术部' AND age>30");
    QPushButton *filterBtn = new QPushButton("应用过滤", this);

    // 分页控件
    QPushButton *prevBtn = new QPushButton("上一页", this);
    m_pageEdit = new QLineEdit(this);
    m_pageEdit->setValidator(new QIntValidator(1, 9999, this));
    m_pageEdit->setFixedWidth(60);
    QPushButton *goBtn = new QPushButton("跳转", this);
    QPushButton *nextBtn = new QPushButton("下一页", this);

    m_pageSizeEdit = new QLineEdit(QString::number(m_pageSize), this);
    m_pageSizeEdit->setValidator(new QIntValidator(10, 500, this));
    m_pageSizeEdit->setFixedWidth(60);

    m_pageInfoLabel = new QLabel("加载中...", this);
    m_statusLabel = new QLabel("", this);

    // 布局管理
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("过滤条件:", this));
    filterLayout->addWidget(m_filterEdit);
    filterLayout->addWidget(filterBtn);

    QHBoxLayout *pageLayout = new QHBoxLayout;
    pageLayout->addWidget(prevBtn);
    pageLayout->addWidget(new QLabel("页码:", this));
    pageLayout->addWidget(m_pageEdit);
    pageLayout->addWidget(goBtn);
    pageLayout->addWidget(nextBtn);
    pageLayout->addSpacing(20);
    pageLayout->addWidget(new QLabel("每页行数:", this));
    pageLayout->addWidget(m_pageSizeEdit);
    pageLayout->addStretch();
    pageLayout->addWidget(m_pageInfoLabel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(m_tableView);
    mainLayout->addLayout(pageLayout);
    mainLayout->addWidget(m_statusLabel);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 信号槽连接
    connect(filterBtn, &QPushButton::clicked, this, &MainWindow::applyFilter);
    connect(prevBtn, &QPushButton::clicked, this, &MainWindow::onPrevPage);
    connect(nextBtn, &QPushButton::clicked, this, &MainWindow::onNextPage);
    connect(goBtn, &QPushButton::clicked, this, &MainWindow::goToPage);
    connect(m_pageSizeEdit, &QLineEdit::editingFinished, this, &MainWindow::changePageSize);
    connect(m_filterEdit, &QLineEdit::returnPressed, this, &MainWindow::applyFilter);

    // 初始加载数据
    loadPageData();
    setWindowTitle("SQLite分页查询演示");
    resize(1000, 600);
}

MainWindow::~MainWindow()
{
    delete m_model;
}

bool MainWindow::initDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("demo.db");

    if (!db.open()) {
        QMessageBox::critical(nullptr, "数据库错误", "连接失败: " + db.lastError().text());
        return false;
    }

    // 创建表
    QSqlQuery query;
    QString createTableSql = QString("CREATE TABLE IF NOT EXISTS %1 ("
                                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                     "name TEXT NOT NULL,"
                                     "age INTEGER NOT NULL,"
                                     "department TEXT NOT NULL,"
                                     "salary REAL NOT NULL,"
                                     "hire_date DATE NOT NULL)")
                                 .arg(m_tableName);

    if (!query.exec(createTableSql)) {
        QMessageBox::critical(nullptr, "创建表错误", "创建表失败: " + query.lastError().text());
        return false;
    }

    // 检查是否已有数据，没有则插入1000条测试数据
    query.exec(QString("SELECT COUNT(*) FROM %1").arg(m_tableName));
    if (query.next() && query.value(0).toInt() == 0) {
        QMessageBox::information(this, "提示", "首次运行，正在插入1000条测试数据...");

        // 使用事务加速插入
        db.transaction();
        query.prepare(QString("INSERT INTO %1 (name, age, department, salary, hire_date) "
                              "VALUES (:name, :age, :department, :salary, :hire_date)")
                          .arg(m_tableName));

        QStringList departments = {"技术部", "市场部", "人事部", "财务部", "销售部"};

        for (int i = 0; i < 1000; ++i) {
            QString name = QString("员工%1").arg(i + 1);
            int age = QRandomGenerator::global()->bounded(20, 61);
            QString dept = departments[QRandomGenerator::global()->bounded(departments.size())];
            double salary = 3000 + QRandomGenerator::global()->bounded(17001);
            QDate hireDate = QDate::currentDate().addDays(
                -QRandomGenerator::global()->bounded(3650));

            query.bindValue(":name", name);
            query.bindValue(":age", age);
            query.bindValue(":department", dept);
            query.bindValue(":salary", salary);
            query.bindValue(":hire_date", hireDate);

            if (!query.exec()) {
                db.rollback();
                QMessageBox::warning(this, "插入数据错误", "插入失败: " + query.lastError().text());
                return false;
            }
        }
        db.commit();
        QMessageBox::information(this, "提示", "测试数据插入完成！");
    }

    return true;
}

int MainWindow::getTotalRowCount()
{
    QSqlQuery query;
    QString sql = QString("SELECT COUNT(*) FROM %1").arg(m_tableName);

    if (!m_currentFilter.isEmpty()) {
        sql += " WHERE " + m_currentFilter;
    }

    if (!query.exec(sql)) {
        qDebug() << "Count query failed:" << query.lastError().text() << "SQL:" << sql;
        return 0;
    }

    if (query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "Total rows:" << count << "Filter:" << m_currentFilter;
        return count;
    }

    return 0;
}

void MainWindow::loadPageData()
{
    m_totalRows = getTotalRowCount();
    int totalPages = m_totalRows > 0 ? (m_totalRows + m_pageSize - 1) / m_pageSize : 1;

    // 确保当前页有效
    if (m_currentPage < 1)
        m_currentPage = 1;
    if (m_currentPage > totalPages && totalPages > 0)
        m_currentPage = totalPages;

    int offset = (m_currentPage - 1) * m_pageSize;
    offset = qMax(0, offset);

    QString sql = QString("SELECT * FROM %1").arg(m_tableName);

    if (!m_currentFilter.isEmpty()) {
        sql += " WHERE " + m_currentFilter;
    }

    sql += QString(" LIMIT %1 OFFSET %2").arg(qMax(1, m_pageSize)).arg(offset);

    m_model->setQuery(sql);

    if (m_model->lastError().isValid()) {
        m_statusLabel->setText("查询错误: " + m_model->lastError().text());
        QMessageBox::critical(this,
                              "查询错误",
                              "查询失败: " + m_model->lastError().text() + "\nSQL: " + sql);
        return;
    }

    // 设置列标题
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "姓名");
    m_model->setHeaderData(2, Qt::Horizontal, "年龄");
    m_model->setHeaderData(3, Qt::Horizontal, "部门");
    m_model->setHeaderData(4, Qt::Horizontal, "工资");
    m_model->setHeaderData(5, Qt::Horizontal, "入职日期");

    updatePageInfo();
    m_pageEdit->setText(QString::number(m_currentPage));
    m_statusLabel->setText(QString("加载成功 - 共 %1 条记录").arg(m_totalRows));
}

void MainWindow::applyFilter()
{
    QString newFilter = m_filterEdit->text().trimmed();

    if (newFilter == m_currentFilter) {
        m_statusLabel->setText("过滤条件未变化");
        return;
    }

    m_currentFilter = newFilter;
    m_currentPage = 1;
    m_statusLabel->setText("应用新过滤条件...");
    loadPageData();
}

void MainWindow::onPrevPage()
{
    if (m_currentPage > 1) {
        m_currentPage--;
        loadPageData();
    } else {
        m_statusLabel->setText("已经是第一页");
    }
}

void MainWindow::onNextPage()
{
    int totalPages = m_totalRows > 0 ? (m_totalRows + m_pageSize - 1) / m_pageSize : 1;

    if (m_currentPage < totalPages) {
        m_currentPage++;
        loadPageData();
    } else {
        m_statusLabel->setText("已经是最后一页");
    }
}

void MainWindow::goToPage()
{
    int page = m_pageEdit->text().toInt();
    if (page < 1) {
        m_statusLabel->setText("页码必须大于0");
        return;
    }

    int totalPages = m_totalRows > 0 ? (m_totalRows + m_pageSize - 1) / m_pageSize : 1;

    if (page <= totalPages) {
        m_currentPage = page;
        loadPageData();
    } else {
        m_statusLabel->setText(QString("页码超出范围，最大页码为%1").arg(totalPages));
    }
}

void MainWindow::changePageSize()
{
    int newSize = m_pageSizeEdit->text().toInt();
    if (newSize <= 0 || newSize > 500) {
        m_statusLabel->setText("每页行数必须在1-500之间");
        m_pageSizeEdit->setText(QString::number(m_pageSize));
        return;
    }

    // 计算新页码，尽量保持在当前记录附近
    int currentRecordPos = (m_currentPage - 1) * m_pageSize;
    m_pageSize = newSize;
    m_currentPage = currentRecordPos / m_pageSize + 1;

    loadPageData();
}

void MainWindow::updatePageInfo()
{
    int totalPages = m_totalRows > 0 ? (m_totalRows + m_pageSize - 1) / m_pageSize : 1;
    int startRow = (m_currentPage - 1) * m_pageSize + 1;
    int endRow = qMin(m_currentPage * m_pageSize, m_totalRows);

    if (m_totalRows == 0) {
        m_pageInfoLabel->setText("没有符合条件的记录");
        return;
    }

    m_pageInfoLabel->setText(QString("共 %1 条记录 | 第 %2/%3 页 | 显示 %4-%5 条")
                                 .arg(m_totalRows)
                                 .arg(m_currentPage)
                                 .arg(totalPages)
                                 .arg(startRow)
                                 .arg(endRow));
}
