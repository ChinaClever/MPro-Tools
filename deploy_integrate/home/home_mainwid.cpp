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
    ui->setupUi(this); //initWid();
    groupBox_background_icon(this); mSsdp = Ssdp_Core::bulid(this);
    connect(mSsdp, &Ssdp_Core::sendMsgSig, this, &Home_MainWid::onDown);
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


void Home_MainWid::onDown(const QString &msg)
{
    QString str = QString::number(++mId) + "、"+ msg + "\n";
    ui->textEdit->moveCursor(QTextCursor::Start);
    ui->textEdit->insertPlainText(str);
}

void Home_MainWid::on_findBtn_clicked()
{
    QStringList ips = mSsdp->searchAll();
    QString str = tr("未找到任何目标设备");
    if(ips.size()) {
        str = tr("已找到%1个目标IP:\t\n").arg(ips.size());
        int i=0; foreach(const auto &ip, ips) {
            str += "   " + ip; i++;
            if(i%4==0) str += "\n";
        }
    }
    MsgBox::information(this, str);
}
