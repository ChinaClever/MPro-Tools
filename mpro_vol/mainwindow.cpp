#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSslSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mNavBarWid = new NavBarWid(ui->barWid);
    // QTimer::singleShot(50,this,SLOT(initFunSlot())); //延时初始化
    connect(mNavBarWid, SIGNAL(navBarSig(int)), this, SLOT(navBarSlot(int)));

    // 查詢Qt支持的版本

    bool bSupp = QSslSocket::supportsSsl();
    QString buildVersion = QSslSocket::sslLibraryBuildVersionString();
    QString version = QSslSocket::sslLibraryVersionString();
    qDebug() << bSupp << buildVersion << version;
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initFunSlot()
{
    initWid(); bool bSupp = QSslSocket::supportsSsl();
    if(!bSupp) MsgBox::critical(this, tr("缺少ssl库，无法进行升级"));
}
void MainWindow::initWid()
{
    mHomeWid = new Home_MainWid(ui->stackedWid);
    ui->stackedWid->addWidget(mHomeWid);

    mSetupWid = new Setup_MainWid(ui->stackedWid);//服务设置
    ui->stackedWid->addWidget(mSetupWid);

    mLog = new LogMainWid(ui->stackedWid);//日志记录
    ui->stackedWid->addWidget(mLog);
}
void MainWindow::navBarSlot(int id)
{
    ui->stackedWid->setCurrentIndex(id);
}

void MainWindow::setDevSlot()
{
    navBarSlot(1);
}
