#ifndef CORE_SENDER_H
#define CORE_SENDER_H

#include "dtls_sender.h"
#include "core_ssdp.h"

class Core_Sender : public QThread
{
    Q_OBJECT
    explicit Core_Sender(QObject *parent = nullptr);
public:
    static Core_Sender *bulid(QObject *parent = nullptr);
    bool sendFile(const QStringList &ips, const QString &fn, const sFileTrans &it);
    bool isFinsh() {return !QThreadPool::globalInstance()->activeThreadCount();}
    void stopRun() {isRun=false; Dtls_Sender::setRunState(isRun);}

signals:
    void devListSig(const QStringList &);
    void errorMessage(const QString &);
    void subProgress(const QString &,int);
    void infoMessage(bool,const QString &);
    void finishSig(bool, const QString &);
    void throwSig(const QString &);
    void overSig();

protected slots:
    void run();
    void timeoutDone();

private:    
    void appendSender();
    void initSender(Dtls_Sender *sender);

private:
    bool isRun;
    QTimer *timer;
    QByteArray mArray;
    QStringList mHosts;
    sFileTrans mFileTrans;
};

#endif // CORE_SENDER_H
