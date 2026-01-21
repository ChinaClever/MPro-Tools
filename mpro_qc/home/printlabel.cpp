#include "printlabel.h"
#include "http/JQLibrary/JQNet"
#include "http/httpclient.h"
#include <QThread>
printLabel::printLabel() {}

QString printLabel::toIni(sLabelData* data)
{
    // 表头
    QString header = "DT,SN,ON,PN,MAC,UUID,QR";

    QString qr = data->qrcode;
    if (qr.isEmpty()) {
        QString base = "https://your-domain.com/label";
        qr = QString("%1?DT=%2&ON=%3&PN=%4&SN=%5&UUID=%6&MAC=%7&CN=%8")
                 .arg(base).arg(data->on).arg(data->pn).arg(data->sn).arg(data->uuid).arg(data->mac).arg(data->cn);
    }

    // 数据行
    QString line = QString("%1,%2,%3,%4,%5,%6")
                       .arg(data->dt)
                       .arg(data->on)
                       .arg(data->pn)
                       .arg(data->sn)
                       .arg(data->mac)
                       .arg(data->uuid)
                       .arg(qr);

    return httpPostIni(header + "\n" + line, "17", "81");
}



QString printLabel::httpPostIni(const QString& data,const QString ip, const QString& host) {
    // 构造 URL
    QString url = QString("http://%1:%2/Integration/Mpro/Execute").arg("192.168.1." + ip).arg(host);
    qDebug() << "URL:" << url;
    qDebug() << "Data:" << data;
    QString str = "";

    // 使用智能指针管理线程和 HttpClient
    QSharedPointer<QThread> thread(new QThread);
    QSharedPointer<AeaQt::HttpClient> http(new AeaQt::HttpClient);

    QEventLoop loop; // 用于等待异步回调
    http->clearAccessCache();
    http->clearConnectionCache();

    QByteArray json = data.toUtf8();
    http->post(url)
        .header("content-type", "plain")
        .onSuccess([&](QString result) {
            qDebug() << "result" << result;
            str = result;
            loop.quit();
        })
        .onFailed([&](QString error) {
            qDebug() << "error" << error;
            str = error;
            loop.quit();
        })
        .onTimeout([&](QNetworkReply*) {
            qDebug() << "http_post timeout";
            str = "timeout";
            loop.quit();
        })
        .timeout(5000) // 增加超时到 5 秒
        .body(json)
        .exec();

    // 将 http 对象移到线程
    http->moveToThread(thread.data());
    QObject::connect(thread.data(), &QThread::started, []() { /* 确保线程启动 */ });
    QObject::connect(thread.data(), &QThread::finished, &loop, &QEventLoop::quit);

    thread->start();
    loop.exec(); // 等待回调完成

    // 确保线程停止
    if (thread->isRunning()) {
        thread->quit();
        thread->wait(1000); // 等待最多 1 秒
    }

    return str;
}
