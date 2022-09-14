/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */

#include "ssdp_obj.h"
#include <QHostInfo>

Ssdp_Obj::Ssdp_Obj(QObject *parent) : QObject(parent)
{
    mPort = 9126; get_local_ip();
    mSocket = new QUdpSocket(this);
    qRegisterMetaType<sSdpIt>("sSdpIt");
    mAddress = QHostAddress("239.255.43.21");
   // auto ok = mSocket->bind(QHostAddress::AnyIPv4, mPort, QUdpSocket::ShareAddress);
    auto ok = mSocket->bind(QHostAddress("192.168.1.215"), mPort, QUdpSocket::ShareAddress);
    if(ok) ok = mSocket->joinMulticastGroup(mAddress);
    if(ok) connect(mSocket,SIGNAL(readyRead()),this,SLOT(readMsgSlot()));
    else cout << mSocket->errorString();
}

QString Ssdp_Obj::get_local_ip()
{
    QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
    foreach(QHostAddress address,info.addresses()) {  // 找出一个IPv4地址即返回
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            mIps << address.toString();
    }
    return QHostAddress(QHostAddress::LocalHost).toString();
}

void Ssdp_Obj::readMsgSlot()
{
    QByteArray reply; QHostAddress host;
    while (mSocket->hasPendingDatagrams()) {
        reply.resize(mSocket->pendingDatagramSize());
        mSocket->readDatagram(reply.data(), reply.size(), &host);
        if(!mIps.contains(host.toString())) recvMsg(reply);
        qDebug() << "CCCCCCCCCC" << host.toString() << reply;
    }
}

bool Ssdp_Obj::write(const QVariant &var)
{
    bool ret = true;
    auto rcv = mSocket->writeDatagram(var.toByteArray(), mAddress, mPort);  mSocket->flush();
    if(rcv < 0) { ret = false; cout << "Error: SSDP write" << mSocket->errorString();}
    return ret;
}

bool Ssdp_Obj::send(const sSdpIt &it)
{
    QByteArray array; QDataStream in(&array, QIODevice::WriteOnly);
    in << it.version << it.fc <<  it.room << it.ip << it.target << it.data << END_CRC;
    return write(array);
}

void Ssdp_Obj::recvMsg(QByteArray &array)
{
    QDataStream out(&array, QIODevice::ReadOnly); sSdpIt it;
    out >> it.version >> it.fc >> it.room >> it.ip >> it.target >> it.data >> it.crc;
    if(it.crc == END_CRC) {
        if(it.fc) emit recvSig(it);
        else rplySearchTarget(it);
    } else cout << it.fc << it.crc << array.size();
}
