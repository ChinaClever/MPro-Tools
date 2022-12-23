/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_serialnumber.h"

Core_SerialNumber::Core_SerialNumber(QObject *parent)
    : QThread{parent}
{
    initCurrentNum();
}

void Core_SerialNumber::writeMac(const QByteArray &mac)
{
    QFile file("usr/data/clever/cfg/mac.ini");
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(mac); file.close();
    } wirteCfgMac();
}


void Core_SerialNumber::wirteCfgMac()
{
    sMac *it = MacAddr::bulid()->macItem;
    CfgCom *cfg = CfgCom::bulid();
    cfg->writeCfg("mac", it->mac, "Mac");
    cfg->writeCfg("cnt", it->cntMac, "Mac");
    cfg->writeCfg("start", it->startMac, "Mac");
    cfg->writeCfg("end", it->endMac, "Mac");
}

QString Core_SerialNumber::updateMacAddr(int step)
{
    sMac *it = MacAddr::bulid()->macItem;
    if(it->mac.size() > 5) {
        MacAddr *mac = MacAddr::bulid();
        it->mac = mac->macAdd(it->mac, step);
        writeMac(it->mac.toLatin1());
        CfgCom::bulid()->writeCfg("mac", it->mac, "Mac");
    } else {
        qDebug() << "updateMacAddr err" << it->mac;
    }

    return it->mac;
}

QString Core_SerialNumber::createSn()
{
    QString cmd = "2I3"; mCurrentNum +=1;
    int m = QDate::currentDate().month();
    int y = QDate::currentDate().year() - 2020;
    for(int i=0; i<3; ++i) cmd += "%" + QString::number(i+1);
    QString sn  = QString(cmd).arg(m, 1, 16).arg(y)
            .arg(mCurrentNum, 5, 10, QLatin1Char('0'));
    this->setCurrentNum();
    return sn.toUpper();
}


void Core_SerialNumber::setDate()
{
    QString value = QDate::currentDate().toString("yyyy-MM-dd");
    CfgCom::bulid()->writeCfg("date", value, "Date");
}

bool Core_SerialNumber::getDate()
{
    QString str = CfgCom::bulid()->readCfg("date","","Date").toString();
    bool ret = false; if(!str.isEmpty()) {
        QDate date = QDate::fromString(str, "yyyy-MM-dd");
        QDate cu = QDate::currentDate();
        date.setDate(date.year(), date.month(), cu.day());
        if(cu > date) ret = true;
    }

    return ret;
}

void Core_SerialNumber::setCurrentNum()
{
    setDate(); CfgCom *cfg = CfgCom::bulid();
    cfg->writeCfg("num", mCurrentNum, "Date");
}

void Core_SerialNumber::initCurrentNum()
{
    bool ret = getDate();
    if(ret) {
        mCurrentNum = 0;
        setCurrentNum();
    } else {
        CfgCom *cfg = CfgCom::bulid();
        int value = cfg->readCfg("num", 1,"Date").toInt();
        mCurrentNum = value;
    }
}
