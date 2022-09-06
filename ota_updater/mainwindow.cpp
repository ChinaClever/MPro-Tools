/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jsonrpcclient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mNavBarWid = new NavBarWid(ui->barWid);
    QTimer::singleShot(50,this,SLOT(initFunSlot())); //延时初始化
    connect(mNavBarWid, SIGNAL(navBarSig(int)), this, SLOT(navBarSlot(int)));
    //qDebug() << "QSslSocket=" << QSslSocket::sslLibraryBuildVersionString();
    //qDebug() << "OpenSSL支持情况:" << QSslSocket::supportsSsl();


//    JsonRpcClient *c = new JsonRpcClient(this);
//     c->startClient("127.0.0.1", 9224); // 127.0.0.1
//     qDebug() << c->pduLogFun(3, 1, 0, 0);


//    c->startClient("192.168.1.99"); // 127.0.0.1

//    c->pduSetData(0, 1, 2, 2, 1, 9);
//    c->pduMetaData(0, 1, 2, 2, 1);


//    c->invokeMethodSync();
//    c->invokeStringMethodSync();
//    c->invokeNotification();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initFunSlot()
{
    mHomeWid = new Home_MainWid(ui->stackedWid);
    ui->stackedWid->addWidget(mHomeWid);

    mStatus = new Pro_StatusWid(ui->stackedWid);
    ui->stackedWid->addWidget(mStatus);
    connect(mHomeWid, SIGNAL(startSig()), mStatus, SLOT(startSlot()));

    mResult = new Pro_ResultWid(ui->stackedWid);
    ui->stackedWid->addWidget(mResult);
    connect(mHomeWid, SIGNAL(startSig()), mResult, SLOT(startSlot()));

    mRecvWid = new Remote_RecvWid(ui->stackedWid);
    ui->stackedWid->addWidget(mRecvWid);
    connect(mHomeWid, SIGNAL(startSig()), mRecvWid, SLOT(startSlot()));
}

void MainWindow::navBarSlot(int id)
{
    ui->stackedWid->setCurrentIndex(id);
}
