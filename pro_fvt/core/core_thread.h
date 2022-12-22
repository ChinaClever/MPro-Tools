#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "core_http.h"
#include "cfgcom.h"

class Core_Thread : public QThread
{
    Q_OBJECT
public:
    explicit Core_Thread(QObject *parent = nullptr);
    void setIps(const QStringList &ips) {m_ips = ips;}
    QString updateMacAddr(int step=1);
    QString createSn();

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);    
    void overSig();

protected:
    void run();
    void workDown();
    void writeMac(const QByteArray &mac);

private:
    bool searchDev();

private:
    QStringList m_ips;
};

#endif // CORE_THREAD_H
