#ifndef SSDP_OBJ_H
#define SSDP_OBJ_H

#include <QtCore>
#include <QUdpSocket>
#include <QHostAddress>
#include "print.h"

#define START_HEAD ((ushort)0xC5C5)
#define END_CRC ((ushort)0x5C5C)

struct sSdpIt{
    sSdpIt():version(1){}
    int fc;
    int version;
    QString room;
    QString ip;
    QString target;
    QByteArray data;
    ushort crc;
};

class Ssdp_Obj : public QObject
{
    Q_OBJECT
public:
    explicit Ssdp_Obj(QObject *parent = nullptr);
     bool send(const sSdpIt &it);

signals:
    void recvSig(const sSdpIt &it);

private:
    QString get_local_ip();
    bool write(const QVariant &var);
    void recvMsg(QByteArray &array);

protected slots:
    virtual void readMsgSlot();
    virtual bool rplySearchTarget(const sSdpIt &it)=0;

private:
    int mPort;
    QStringList mIps;
    QUdpSocket *mSocket;
    QHostAddress mAddress;
};

#endif // SSDP_OBJ_H
