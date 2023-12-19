/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_mainwid.h"
#include "ui_home_mainwid.h"

Home_MainWid::Home_MainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_MainWid)
{
    ui->setupUi(this); initWid();
    groupBox_background_icon(this);
    mWorkWid = new Home_WorkWid(ui->workWid);
    Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
    connect(ssdp, &Ssdp_Core::sendMsgSig, this, &Home_MainWid::onDown);
    connect(mWorkWid, &Home_WorkWid::startSig, this, &Home_MainWid::onStart);
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}


void Home_MainWid::initWid()
{
    QPalette pl = ui->textEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
}

void Home_MainWid::onStart()
{
    ui->textEdit->clear();
    mId = 0;
}

void Home_MainWid::onDown(const QString &msg)
{
    QString str = QString::number(++mId) + "、"+ msg + "\n";
    ui->textEdit->moveCursor(QTextCursor::Start);
    ui->textEdit->insertPlainText(str);
}
