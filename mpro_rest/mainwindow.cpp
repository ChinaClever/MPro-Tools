/*
 *
 *  Created on: 2022年1月1日
 *      Author: Lzy
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mNavBarWid = new NavBarWid(ui->barWid);
    QTimer::singleShot(50,this,SLOT(initFunSlot())); //延时初始化
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initFunSlot()
{
    /*
    QJsonObject json
    {
        {"id", 0}
    };






    static AeaQt::HttpClient client;
    client.get("https://192.168.1.163:3166/pduMetaData")
            .sslConfiguration(SSLConfig)
            .header("content-type", "application/json")
            .body(json)
            .onSuccess([](QString result){qDebug()<<result;})
    .onFailed([](QString error){qDebug()<<error;})
    .exec();
*/
    mHomeWid = new Home_MainWid(ui->stackedWid);
    ui->stackedWid->addWidget(mHomeWid);
}


