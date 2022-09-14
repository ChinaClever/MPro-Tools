/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */

#include "ssdp_search.h"

Ssdp_Search::Ssdp_Search(QObject *parent) : Ssdp_Obj(parent)
{
    connect(this, &Ssdp_Obj::recvSig, this, &Ssdp_Search::recvSlot);
}


QStringList Ssdp_Search::searchDev(const QString &ip)
{
    return searchTarget("", ip);
}

QStringList Ssdp_Search::searchRoom(const QString &room)
{
    return searchTarget(room, "");
}


void Ssdp_Search::recvSlot(const sSdpIt &it)
{
    if(!it.fc) {
        mSet << it.ip;
        emit targetSig(it.ip);
    }
}

QStringList Ssdp_Search::respondList()
{
    int cnt = 0; mSet.clear();
    QByteArray reply; QHostAddress host;
    for(int i=0; i<1000; i+=100) {
        if(mSet.size() != cnt) {
            cnt = mSet.size();
            i = 700;
        } cm_mdelay(100);
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
    it.data = message.toLatin1(); bool ret = send(it);
    if(ret) ls = respondList();
    return ls;
}

bool Ssdp_Search::rplySearchTarget(const sSdpIt &it)
{
    return true;
}
