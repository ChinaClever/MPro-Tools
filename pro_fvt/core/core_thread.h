#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "core_serialnumber.h"

class Core_Thread : public Core_SerialNumber
{
    Q_OBJECT
public:
    explicit Core_Thread(QObject *parent = nullptr);
    void setIps(const QStringList &ips) {m_ips = ips;}
    QStringList getFs();
    void run();

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);    
    void overSig();

protected:
    void timeSync();
    bool workDown(const QString &ip);

private:
    bool searchDev();
    bool fsCheck();

private:
    QStringList m_ips;
};

#endif // CORE_THREAD_H
