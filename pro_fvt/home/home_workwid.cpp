/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QStandardPaths>
#include <QFileDialog>
#include "filemgr.h"

Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this); initFun();
    mHttp = Core_HttpObj::bulid(this);
    mHttp->initHost("192.168.1.89");
}

Home_WorkWid::~Home_WorkWid()
{
    delete ui;
}

void Home_WorkWid::initFun()
{
    mDir = "usr/data/clever/cfg/"; FileMgr::build().mkpath(mDir); m_fs.clear();
    m_fs << mDir+"alarm.cfg" << mDir+"devParam.ini" << mDir+"cfg.ini" <<  mDir+"logo.png";
    m_fs << mDir+"inet.ini" << mDir+"alarm.df" << mDir+"snmpd.conf" << mDir+"mac.ini";
}

void Home_WorkWid::on_startBtn_clicked()
{
    emit startSig();

    mHttp->uploadFile(m_fs);
}

void Home_WorkWid::writeMac(const QByteArray &mac)
{
    QFile file(mDir+"mac.ini");
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(mac); file.close();
    }
}


void Home_WorkWid::on_findBtn_clicked()
{
    QString room, ip; //getInput(room, ip);
    Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
    QStringList ips = ssdp->searchTarget(room, ip);
    QString str = tr("未找到任何目标设备");
    if(ips.size()) {
        str = tr("已找到%1个设备，包含%2个主机，%3个副机\t\n")
                .arg(ips.size() + ssdp->getSlaveNum())
                .arg(ips.size()).arg(ssdp->getSlaveNum());
        int i=0; foreach(const auto &ip, ips) {
            str += "   " + ip; i++;
            if(i%4==0) str += "\n";
        }
    }
    MsgBox::information(this, str);
}

void Home_WorkWid::on_downBtn_clicked()
{
    QString str = tr("请确认下载设备的配置文件?");
    if(MsgBox::question(this, str)) {
        FileMgr::build().delFileOrDir(mDir);
        emit startSig(); initFun();
        QStringList fs = m_fs;
        fs.removeLast();
        mHttp->downFile(fs);
    }
}

