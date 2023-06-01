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
