/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    Core_Http::bulid(this);
}

QStringList Core_Thread::getFs()
{
    FileMgr::build().mkpath("usr/data/clever/doc/");
    QString dir = "usr/data/clever/cfg/"; FileMgr::build().mkpath(dir);
    QStringList fs; fs << "usr/data/clever/ver.ini" << "usr/data/clever/doc/modbus.xlsx";
    fs << dir+"alarm.cfg" << dir+"devParam.ini" << dir+"cfg.ini" << dir+"inet.ini";
    fs << dir+"alarm.df" << dir+"snmpd.conf" << dir+"logo.png" << dir+"mac.ini";
    fs << dir + "sn.conf";
    return fs;
}

bool Core_Thread::fsCheck()
{
    bool ret = true;
    QStringList fs = getFs();
    foreach (const auto fn, fs) {
        if(!QFile::exists(fn)) {
            ret = false;
            emit msgSig(tr("文件未找到")+fn, ret);
        }
    } if(ret) emit msgSig(tr("文件未检查 OK!"), ret);
    return ret;
}


bool Core_Thread::searchDev()
{
    bool ret = true; if(m_ips.isEmpty()) {
        Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
        QStringList ips = ssdp->searchAll();
        QString str = tr("未找到任何目标设备"); // cm_mdelay(150);
        if(ips.size()) str = tr("已找到%1个设备").arg(ips.size());
        else {ret = false;} m_ips = ips;
        emit msgSig(str, ret);
    }
    return ret;
}

void Core_Thread::timeSync()
{
    QString fmd = "yyyy-MM-dd hh:mm:ss";
    QString t = QDateTime::currentDateTime().toString(fmd);
    emit msgSig(tr("时间设置:")+t, true);
    Core_Http *http = Core_Http::bulid(this);
    sCfgItem it; it.type = 43; it.fc =1;
    http->setting(it, t); cm_mdelay(320);
}

bool Core_Thread::workDown(const QString &ip)
{
    bool res = true;
    emit msgSig(tr("目标设备:")+ip, true);
    Core_Http *http = Core_Http::bulid(this);
    http->initHost(ip); QStringList fs = getFs();
    fs.removeFirst(); timeSync();
    foreach (const auto fn, fs) {
        bool ret = http->uploadFile(fn);
        if(!ret) res = false;
        emit msgSig(fn, ret);
        cm_mdelay(220);
    }

    if(res) {
        emit msgSig(tr("设备重启，设备有响声"), true);
        http->execute("sync"); cm_mdelay(1000);
        http->execute("reboot"); // killall cores
    }
    return res;
}

void Core_Thread::run()
{
    bool ret = searchDev();
    if(ret && fsCheck()) {
        foreach (const auto &ip, m_ips) {
            ret = workDown(ip); cm_mdelay(150);
            emit finshSig(ret, ip+" ");            
        }m_ips.clear();
    } emit overSig();
}
