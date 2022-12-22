#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "core_http.h"

class Core_Thread : public QThread
{
    Q_OBJECT
public:
    explicit Core_Thread(QObject *parent = nullptr);

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);
    void overSig();

protected:
    void run();
    void workDown();

private:
    bool searchDev();

private:
    QStringList m_ips;
};

#endif // CORE_THREAD_H
