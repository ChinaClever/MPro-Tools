/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#endif

#include "ssdp_obj.h"

#include <QNetworkInterface>
#include <QSet>
#include <QtEndian>

namespace {

bool ipv4Up(const QNetworkInterface &iface)
{
    const auto flags = iface.flags();
    return flags.testFlag(QNetworkInterface::IsUp)
            && !flags.testFlag(QNetworkInterface::IsLoopBack);
}

bool usableIpv4(const QHostAddress &ip)
{
    return ip.protocol() == QAbstractSocket::IPv4Protocol
            && !ip.isLoopback()
            && ip.toIPv4Address() != 0;
}

// 指定组播出接口。多网卡时不设置的话，报文只会从默认路由那块网卡出去。
bool setMulticastIfAddr(QUdpSocket &sock, const QHostAddress &ipv4)
{
    const qintptr fd = sock.socketDescriptor();
    if (fd < 0)
        return false;

    const quint32 addr = qToBigEndian(ipv4.toIPv4Address());
#if defined(Q_OS_WIN)
    const int rc = ::setsockopt(SOCKET(fd), IPPROTO_IP, IP_MULTICAST_IF,
                                reinterpret_cast<const char *>(&addr), int(sizeof(addr)));
#else
    const int rc = ::setsockopt(int(fd), IPPROTO_IP, IP_MULTICAST_IF,
                                &addr, socklen_t(sizeof(addr)));
#endif
    return rc == 0;
}

bool sendOnIpv4(const QByteArray &array, const QHostAddress &localIp,
                const QNetworkInterface &iface, const QHostAddress &group, quint16 port)
{
    QUdpSocket sock;
    const auto share = QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint;
    if (!sock.bind(localIp, 0, share) && !sock.bind(localIp, 0)) {
        cout << "Error: SSDP bind" << localIp.toString() << iface.humanReadableName() << sock.errorString();
        return false;
    }

    sock.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
    sock.setMulticastInterface(iface);
    if (!setMulticastIfAddr(sock, localIp))
        cout << "Error: SSDP multicast if" << localIp.toString() << iface.humanReadableName();

    const qint64 n = sock.writeDatagram(array, group, port);
    sock.flush();
    if (n < 0) {
        cout << "Error: SSDP write" << localIp.toString() << iface.humanReadableName() << sock.errorString();
        return false;
    }
    return true;
}

}

Ssdp_Obj::Ssdp_Obj(QObject *parent) : QObject(parent)
{
    mPort = 9126;
    mUdp = new Net_Udp(this);
    mSocket = new QUdpSocket(this);
    qRegisterMetaType<sSdpIt>("sSdpIt");
    mAddress = QHostAddress("239.255.43.21");
}

bool Ssdp_Obj::ssdpBind()
{
    auto ok = mSocket->bind(QHostAddress::AnyIPv4, mPort, QUdpSocket::DontShareAddress);
    if(ok) ok = mSocket->joinMulticastGroup(mAddress);
    if(ok) connect(mSocket,SIGNAL(readyRead()),this,SLOT(readMsgSlot()));
    else cout << mSocket->errorString();
    return ok;
}

bool Ssdp_Obj::udpBind()
{
    // 绑定 0.0.0.0。双栈 Any 在 Windows 多网卡时，经常收不到非默认网卡上的 IPv4 回复。
    bool ret = mUdp->bind(mPort+1, QHostAddress::AnyIPv4);
    if(ret) connect(mUdp,&Net_Udp::recvSig,this,&Ssdp_Obj::recvUdpSlot);
    return ret;
}

void Ssdp_Obj::readMsgSlot()
{
    QByteArray reply; QHostAddress host;
    while (mSocket->hasPendingDatagrams()) {
        reply.resize(mSocket->pendingDatagramSize());
        mSocket->readDatagram(reply.data(), reply.size(), &host);
        recvMsg(host.toString(), reply);  cout << host.toString();
    }
}

QByteArray Ssdp_Obj::toArray(const sSdpIt &it)
{
    QByteArray array; QDataStream in(&array, QIODevice::WriteOnly);
    in << it.version << it.fc <<  it.room << it.ip << it.target << it.data << END_CRC;
    return qCompress(array);
}

bool Ssdp_Obj::toItem(const QByteArray &array, sSdpIt &it)
{
    QByteArray data = qUncompress(array);
    bool ret = false; if(data.isEmpty()) return ret;
    QDataStream out(&data, QIODevice::ReadOnly);
    out >> it.version >> it.fc >> it.room >> it.ip >> it.target >> it.data >> it.crc;
    if(it.crc == END_CRC) ret = true;
    return ret;
}

bool Ssdp_Obj::ssdpSend(const sSdpIt &it)
{
    // 每块正在使用的 IPv4 网卡各发一次，源地址就是该网卡地址，PDU 能按原路回复。
    const QByteArray array = toArray(it);
    bool ret = false;
    QSet<QString> used;
    const auto ifaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : ifaces) {
        if (!ipv4Up(iface))
            continue;
        const auto entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            const QHostAddress ip = entry.ip();
            if (!usableIpv4(ip))
                continue;
            const QString key = ip.toString();
            if (used.contains(key))
                continue;
            used.insert(key);
            if (sendOnIpv4(array, ip, iface, mAddress, quint16(mPort)))
                ret = true;
        }
    }

    if (!ret) {
        const qint64 rcv = mSocket->writeDatagram(array, mAddress, quint16(mPort));
        mSocket->flush();
        if (rcv < 0)
            cout << "Error: SSDP write" << mSocket->errorString();
        else
            ret = true;
    }
    return ret;
}

bool Ssdp_Obj::udpSend(const QHostAddress &host, const sSdpIt &it)
{
    QByteArray array = toArray(it);
    return mUdp->writeDatagram(array, host, mPort+1);
}

void Ssdp_Obj::recvMsg(const QString &ip, const QByteArray &array)
{
    sSdpIt it; if(toItem(array, it)) {
        if(!ip.contains(it.ip)) {it.ip = ip; it.ip.remove("::ffff:");}
        emit recvSig(it); //cout << ip;
    } else cout << it.fc << it.crc << array.size();
}

