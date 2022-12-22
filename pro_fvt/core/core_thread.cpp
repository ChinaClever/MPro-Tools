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

bool Core_Thread::searchDev()
{
    QString room, ip; bool ret = true;
    Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
    QStringList ips = ssdp->searchTarget(room, ip);
    QString str = tr("未找到任何目标设备");
    if(ips.size()) str = tr("已找到%1个设备").arg(ips.size());
    else {ret = false;} m_ips = ips;
    emit msgSig(str, ret);
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
