#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "core_object.h"

class Core_Thread : public Core_Object
{
    Q_OBJECT
    explicit Core_Thread(QObject *parent = nullptr);
public:
    static Core_Thread *bulid(QObject *parent = nullptr);
    void setIps(const QStringList &ips) {m_ips = ips;}
    void run();

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);    
    void overSig();

private:
    bool snCheck();
    bool macCheck();
    bool searchDev();
    bool timeCheck();
    bool devNumCheck();
    bool outletCheck();
    bool outputVolCheck();
    bool supplyVolCheck();
    bool parameterCheck();
    bool thresholdCheck();
    bool workDown(const QString &ip);


private:
    QStringList m_ips;
};

#endif // CORE_THREAD_H
