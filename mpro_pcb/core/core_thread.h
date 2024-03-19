#ifndef CORE_THREAD_H
#define CORE_THREAD_H
#include "json_pack.h"
#include "core_object.h"
#include "serialstatuswid.h"
#include "setup_mainwid.h"

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
    void serilSig();
private:
    bool snCheck();
    bool envCheck();
    bool searchDev();
    bool timeCheck();
    bool workDown();
    bool onserial(SerialPort  *Item);
private:
    QString mLogo;
    QStringList m_ips;
    SerialStatusWid *mComWid;
    SerialPort *mSerial;
    sCfgComIt *mItem;
};

#endif // CORE_THREAD_H
