/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"

Core_Thread::Core_Thread(QObject *parent)
    : QThread{parent}
{
    Ssdp_Core::bulid(this);
}

void Core_Thread::writeMac(const QByteArray &mac)
{
    QFile file("usr/data/clever/cfg/mac.ini");
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(mac); file.close();
    }
}

QString Core_Thread::updateMacAddr(int step)
{
    sMac *it = MacAddr::bulid()->macItem;
    if(it->mac.size() > 5) {
        MacAddr *mac = MacAddr::bulid();
        it->mac = mac->macAdd(it->mac, step); writeMac(it->mac.toLatin1());
        ///BaseLogs::bulid()->writeMac(it->mac);  ////////==============
        CfgCom::bulid()->writeCfg("mac", it->mac, "Mac");
    } else {
        qDebug() << "updateMacAddr err" << it->mac;
    }

    return it->mac;
}

QString Core_Thread::createSn()
{
    static int currentNum = 1;
    QString cmd = "2I3";
    //mItem->currentNum +=1;  ///////==========
    int m = QDate::currentDate().month();
    int y = QDate::currentDate().year() - 2020;
    for(int i=0; i<3; ++i) cmd += "%" + QString::number(i+1);
    QString sn  = QString(cmd).arg(m, 1, 16).arg(y)
            .arg(currentNum, 5, 10, QLatin1Char('0'));
    //Cfg::bulid()->setCurrentNum(); ////==========
    return sn.toUpper();
}



bool Core_Thread::searchDev()
{
    bool ret = true;
    if(m_ips.isEmpty()) {
        QString room, ip;
        Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
        QStringList ips = ssdp->searchTarget(room, ip);
        QString str = tr("未找到任何目标设备");
        if(ips.size()) str = tr("已找到%1个设备").arg(ips.size());
        else {ret = false;} m_ips = ips;
        emit msgSig(str, ret);
    }
    return ret;
}

void Core_Thread::workDown()
{
    bool ret = searchDev();

}

void Core_Thread::run()
{
    workDown();
    emit overSig();
}
