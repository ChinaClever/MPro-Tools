/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */

#include "ssdp_search.h"

Ssdp_Search::Ssdp_Search(QObject *parent) : Ssdp_Obj(parent)
{   
    connect(this, &Ssdp_Obj::recvSig, this, &Ssdp_Search::searchRecvSlot);
}


QStringList Ssdp_Search::searchDev(const QString &ip)
{
    return searchTarget("", ip);
}

QStringList Ssdp_Search::searchRoom(const QString &room)
{
    return searchTarget(room, "");
}


void Ssdp_Search::searchRecvSlot(const sSdpIt &it)
{
    if(!it.fc) {
        mSet << it.ip;
        mSlaveNum += it.data.toInt();
        emit targetSig(it.ip);
    }
}

QStringList Ssdp_Search::respondList()
{
    int cnt = 0; mSet.clear(); mSlaveNum = 0;
    QByteArray reply; QHostAddress host;
    for(int i=0; i<500; i+=10) {
        if(mSet.size() != cnt) {
            cnt = mSet.size();
            i = 100;
        } cm_mdelay(10);
    }

    return mSet.values();
}

QStringList Ssdp_Search::searchTarget(const QString &room, const QString &ip)
{
    QStringList ls;
    QString message("M-SEARCH * HTTP/1.1\r\n"        \
                    "Host:239.255.43.21:1900\r\n" \
                    "ST:urn:schemas-upnp-org:device:pdu:\r\n" \
                    "Man:\"ssdp:discover\"\r\n" \
                    "MX:3\r\n" \
                    "\r\n");
    sSdpIt it; it.fc = 0; it.room = room; it.ip = ip;
    it.data = message.toLatin1(); bool ret = ssdpSend(it);
    if(ret) ls = respondList();
    return ls;
}

