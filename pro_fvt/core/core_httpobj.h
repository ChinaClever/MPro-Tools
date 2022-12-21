#ifndef CORE_HTTPOBJ_H
#define CORE_HTTPOBJ_H
#include "ssdp_core.h"
#include "http/httpclient.h"

class Core_HttpObj : public QObject
{
    Q_OBJECT
    explicit Core_HttpObj(QObject *parent = nullptr);
public:
    static Core_HttpObj *bulid(QObject *parent = nullptr);
    void downFile(const QStringList fs, const QString &ip="192.168.1.99", int port=3166);
    void uploadFile(const QStringList fs, const QString &ip="192.168.1.99", int port=3166);
    void setting(const sDataItem &it, const QString &ip="192.168.1.99", int port=3166);
    void setting(const sCfgItem &it, const QVariant &value, const QString &ip="192.168.1.99", int port=3166);

signals:
    void httpSig(const QString &msg);

private:
    auto sslConfig();
    void http_get(const QString &method, QJsonObject &json, const QString &ip, int port);
    void http_post(const QString &method, QJsonObject &json, const QString &ip, int port);
    void http_put(const QString &method, QJsonObject &json, const QString &ip, int port);
    void http_upload(const QString &method, const QString &file, const QString &ip, int port);
    void http_down(const QString &method, QJsonObject &json, const QString &file, const QString &ip, int port);

private:
    AeaQt::HttpClient mHttp;
};

#endif // CORE_HTTPOBJ_H
