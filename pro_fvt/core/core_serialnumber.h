#ifndef CORE_SERIALNUMBER_H
#define CORE_SERIALNUMBER_H

#include "core_http.h"
#include "cfgcom.h"

class Core_SerialNumber : public QThread
{
    Q_OBJECT
public:
    explicit Core_SerialNumber(QObject *parent = nullptr);
    QString updateMacAddr(int step=1);
    QString createSn();

private:
    void wirteCfgMac();
    void writeMac(const QByteArray &mac);

};

#endif // CORE_SERIALNUMBER_H
