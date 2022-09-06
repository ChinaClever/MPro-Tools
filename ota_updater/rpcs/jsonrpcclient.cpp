#include "jsonrpcclient.h"

JsonRpcClient::JsonRpcClient(QObject *parent)
    : JsonRpcObj{parent}
{

}

int JsonRpcClient::pduMetaData(uchar addr,  uchar type, uchar topic, uchar sub, uchar id)
{
    int ret = -1;
    auto result = rpc_client->call("pduMetaData", addr, type, topic, sub, id);
    if (result->isSuccess()) {
        ret = result->result().toInt();
        //qDebug() << "result of pduMetaData RPC call:" << result->result();
    } else {
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

bool JsonRpcClient::pduSetData(uchar addr,  uchar type, uchar topic, uchar sub, uchar id, uint value)
{
    bool ret = false;
    auto result = rpc_client->call("pduSetData", addr, type, topic,sub, id, value);
    if (result->isSuccess()) {
        ret = result->result().toBool();
        //qDebug() << "result of pduSetData RPC call:" << result->result();
    } else {
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

QString JsonRpcClient::pduGetString(uchar addr, uchar fc, uchar id)
{
    QString str;
    auto result = rpc_client->call("pduGetString", addr, fc, id);
    if (result->isSuccess()) {
        str = result->result().toString();
        qDebug() << "result of pduGetString RPC call:" << result->result();
    } else {
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }
    return str;
}

bool JsonRpcClient::pduSetString(uchar addr, uchar fc, uchar id, const QString &str)
{
    bool ret = false;
    auto result = rpc_client->call("pduSetString", addr, fc, id, str);
    if (result->isSuccess()) {
        ret = result->result().toBool();
        qDebug() << "result of pduSetString RPC call:" << result->result();
    } else {
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}


QString JsonRpcClient::pduLogFun(uchar type, uchar fc, int id, int cnt)
{
    QString ret;
    auto result = rpc_client->call("pduLogFun", type, fc, id, cnt);
    if (result->isSuccess()) {
        ret = result->result().toString();
        qDebug() << "result of pduLogFun RPC call:" << result->result();
    } else {
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

void JsonRpcClient::invokeStringMethodSync()
{
    //qsrand(std::time(nullptr)); //////////===========

    auto result = rpc_client->call("printMessage", "hello, world");

    if (result->isSuccess()) {
        qDebug() << "result of synchronous RPC call:" << result->result();
    } else {
        qDebug() << "RPC error:" << result->toString();
    }
}

void JsonRpcClient::invokeNotification()
{
    rpc_client->notification("printNotification", "hello, notification");
}
