#include "core_serialnumber.h"

Core_SerialNumber::Core_SerialNumber(QObject *parent)
    : QThread{parent}
{

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
    //cout << it->mac << it->startMac << it->endMac;
}

QString Core_SerialNumber::updateMacAddr(int step)
{
    sMac *it = MacAddr::bulid()->macItem;
    if(it->mac.size() > 5) {
        MacAddr *mac = MacAddr::bulid();
        it->mac = mac->macAdd(it->mac, step);
        writeMac(it->mac.toLatin1());
        ///BaseLogs::bulid()->writeMac(it->mac);  ////////==============
        CfgCom::bulid()->writeCfg("mac", it->mac, "Mac");
    } else {
        qDebug() << "updateMacAddr err" << it->mac;
    }

    return it->mac;
}

QString Core_SerialNumber::createSn()
{
    static int currentNum = 1;
    QString cmd = "2I3";
    currentNum +=1; //mItem->currentNum +=1;  ///////==========
    int m = QDate::currentDate().month();
    int y = QDate::currentDate().year() - 2020;
    for(int i=0; i<3; ++i) cmd += "%" + QString::number(i+1);
    QString sn  = QString(cmd).arg(m, 1, 16).arg(y)
            .arg(currentNum, 5, 10, QLatin1Char('0'));
    //Cfg::bulid()->setCurrentNum(); ////==========
    return sn.toUpper();
}
