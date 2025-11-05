#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include "backcolour/backcolourcom.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/box_back.jpg");
    initValues = {
        {"264", "90", "90", "264", "63", "0", "0", "50.4", "13.86", "0", "0", "11.088"},
        {"264", "90", "90", "264", "63", "0", "0", "50.4", "13.86", "0", "0", "11.088"}
    };
    headers << "电压最大值"
            << "电压最小值"
            << "电压下临界值"
            << "电压上临界值"
            << "电流最大值"
            << "电流最小值"
            << "电流下临界值"
            << "电流上临界值"
            << "功率最大值"
            << "功率最小值"
            << "功率下临界值"
            << "功率上临界值";
    headers << "全选/全不选";

    setPhase();
    setTem();
    setOutset();

}

void MainWindow::setTem()
{
    int rows = 2;
    int cols = 4;
    initValues = {
        {"40", "0", "0", "40"},
        {"99", "0", "0", "99"}
    };
    QStringList headers;
    headers << "最大值" << "最小值"  << "下临界值" << "上临界值"
            << "全选/全不选";
    setupTable(ui->TemWid, rows, cols, headers, initValues);
}

void MainWindow::setPhase()
{

    int rows = 2;
    int cols = 12;
    setupTable(ui->phaseWid, rows, cols, headers, initValues);
}

void MainWindow::setOutset()
{
    int rows = 42;
    int cols = 6; // 只要电流和功率各 4 列

    // 电流和功率列标题
    QStringList headers;
    headers << "电流最大值" << "电流下临界值" << "电流上临界值"
            << "功率最大值" << "功率下临界值" << "功率上临界值"
            << "全选/全不选"; // 最后一列放按钮

    QVector<QString> firstRow = {"10", "0", "8", "2.2", "0", "1.76"};

    QVector<QVector<QString>> initValues(rows, firstRow);

    setupTable(ui->outsetWid, rows, cols, headers, initValues);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTable(QTableWidget *table, int rows, int cols, const QStringList &headers, const QVector<QVector<QString>> &initValues)
{
    table->setColumnCount(cols + 1); // 最后一列放按钮
    table->setRowCount(rows);
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            QWidget *cellWidget = new QWidget();
            QLineEdit *edit = new QLineEdit();
            QCheckBox *check = new QCheckBox();

            // 设置初始值
            if (!initValues.isEmpty() && row < initValues.size() && col < initValues[row].size())
                edit->setText(initValues[row][col]);
            else
                edit->setText("0"); // 默认值

            check->setChecked(false);

            QHBoxLayout *layout = new QHBoxLayout(cellWidget);
            layout->addWidget(edit);
            layout->addWidget(check);
            layout->setContentsMargins(0, 0, 0, 0);
            table->setCellWidget(row, col, cellWidget);
        }

        // 最后一列放按钮
        QPushButton *btnAll = new QPushButton("全选");
        QPushButton *btnNone = new QPushButton("全不选");
        QWidget *ctrlWidget = new QWidget();
        QHBoxLayout *ctrlLayout = new QHBoxLayout(ctrlWidget);
        ctrlLayout->addWidget(btnAll);
        ctrlLayout->addWidget(btnNone);
        ctrlLayout->setContentsMargins(0, 0, 0, 0);
        table->setCellWidget(row, cols, ctrlWidget);

        connect(btnAll, &QPushButton::clicked, this, [=](){
            for (int c = 0; c < cols; c++) {
                QWidget *cell = table->cellWidget(row, c);
                if (cell) {
                    QCheckBox *chk = cell->findChild<QCheckBox *>();
                    if (chk) chk->setChecked(true);
                }
            }
        });

        connect(btnNone, &QPushButton::clicked, this, [=](){
            for (int c = 0; c < cols; c++) {
                QWidget *cell = table->cellWidget(row, c);
                if (cell) {
                    QCheckBox *chk = cell->findChild<QCheckBox *>();
                    if (chk) chk->setChecked(false);
                }
            }
        });
    }
}


void MainWindow::on_OutsetBtn_clicked()
{
    int n = ui->outSetNum->value(); // SpinBox 指定行数
    int totalRows = ui->outsetWid->rowCount();
    int totalCols = 6; // 电流+功率列数

    for (int row = 0; row < totalRows; ++row) {
        bool check = (row < n); // 前 N 行选中，后面不选
        for (int col = 0; col < totalCols; ++col) {
            QWidget *cell = ui->outsetWid->cellWidget(row, col);
            if (cell) {
                QCheckBox *chk = cell->findChild<QCheckBox *>();
                if (chk) chk->setChecked(check);
            }
        }
    }

}


void MainWindow::createPhase()
{
    createJsonFromRow(ui->phaseWid,0);
}

void MainWindow::createLoop()
{
    createJsonFromRow(ui->phaseWid,1);
}

void MainWindow::createTem()
{
    createJsonFromRow(ui->TemWid,0);
    createJsonFromRow(ui->TemWid,1);
}


void MainWindow::createOutput()
{
    for(int i = 0; i < 48; i ++)
    createJsonFromRow(ui->outsetWid,i);
}

void MainWindow::on_CreateJsonBtn_clicked()
{
    createPhase();
    createLoop();
    createTem();
    createOutput();
}

QString MainWindow::getTopic(int topic)
{
    QString str;
    if(topic == 2) str = "vol";
    else if(topic == 3) str = "cur";
    else if(topic == 4) str = "pow";
    else if(topic == 11) str = "tem";
    else if(topic == 12) str = "hum";
    return str;
}
QString MainWindow::getSubtopic(int subtopic)
{
    QString str;
    if(subtopic == 4) str = "max";
    else if(subtopic == 5) str = "min";
    else if(subtopic == 7) str = "upper";
    else if(subtopic == 6) str = "lower";
    return str;
}

void MainWindow::createJsonFromRow(QTableWidget *table, int row)
{
    if (!table || row >= table->rowCount()) return;
    int type,subtopic,topic;
    // 根据表格类型决定文件夹名
    QString folderName;
    if (table == ui->phaseWid){
        if(row == 0)folderName = "phase", type = 1;
        if(row == 1)folderName = "loop", type = 2;
    }
    else if (table == ui->outsetWid)
        folderName = "outset", type = 3;
    else if (table == ui->TemWid)
        folderName = "temperature", type = 6;
    else
        folderName = "other";

    QString dirPath = QString("Json/%1").arg(folderName);
    QDir dir;
    if (!dir.exists(dirPath)) {
        dir.mkpath(dirPath);  // 递归创建目录
    }

    int cols = table->columnCount() - 1; // 最后一列是按钮
    for (int col = 0; col < cols; ++col) {
        QWidget *cell = table->cellWidget(row, col);
        if (!cell) continue;
        QCheckBox *chk = cell->findChild<QCheckBox *>();
        QLineEdit *edit = cell->findChild<QLineEdit *>();
        if (chk && chk->isChecked() && edit) {
            QJsonObject pduDataSet;
            pduDataSet["addr"] = ui->addr->text().toInt();
            pduDataSet["type"] = type;
            if(type == 1 || type == 2)
                topic = col/4 + 2, subtopic = col%4 + 4;
            else if(type == 6){
                if(row == 0)topic = 11, subtopic = col + 4;
                else topic = 12, subtopic = col%4 + 4;
            }
            else {
                topic = col/3 + 3;
                int x = col % 3;
                if(x == 0)subtopic = 4;
                else if(x == 1)subtopic = 6;
                else if(x == 2)subtopic = 7;
            }
            pduDataSet["topic"] = topic;
            pduDataSet["subtopic"] = subtopic;
            if(folderName != "outset")pduDataSet["id"] = 0;
            else pduDataSet["id"] = row + 1;

            pduDataSet["value"] = edit->text().toDouble();

            QJsonObject root;
            root["pduDataSet"] = pduDataSet;

            QString str = getTopic(topic);
            QString str2 = getSubtopic(subtopic);

//            qDebug()<<str <<' '<<str2;

            QString fileName = QString("%1/%2/%3_%4_%5%6_%7.json").arg("Json").
                    arg(folderName).arg(folderName).arg(str).arg(str2).arg(row+1).arg(ui->addr->text());

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                QJsonDocument doc(root);
                file.write(doc.toJson());
                file.close();
            }
        }
    }
}
