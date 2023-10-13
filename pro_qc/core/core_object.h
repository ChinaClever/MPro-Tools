#ifndef CORE_OBJECT_H
#define CORE_OBJECT_H

#include "core_http.h"
#include "cfgcom.h"

struct sParameter {
    uint devSpec; // 设备规格 A\B\C\D
    uint language; // 0 中文 1 英文
    uint isBreaker; // 0没有断路器 1有断路器
    uint sensorBoxEn; // 传感器盒子 0：禁用， 1：启用
    uint vh; // 0:垂直 1:水平
    uint standNeutral; // 0-标准,1-中性
    uint webBackground; // 网页背景颜色
    uint supplyVol;

    uint lineNum; //设备单三相
    uint boardNum;   //　执行板数量
    uint loopNum; // 回路数量
    uint outputNum;   //　输出位数量
};

struct sThreshold
{
    double lineVol;
    double lineCur;
    double linePow;

    double loopVol;
    double loopCur;
    double loopPow;
    double volValue; //电压参考值
    QVariantList ops;
    QVariantList outputVols;
};

struct sCoreUnit
{
    sParameter param;
    sThreshold rate;
};


struct sCoreItem
{
    int port = 3166;
    QString ip="192.168.1.163";
    QString logo="logo.png";
    sCoreUnit desire; // 期望
    sCoreUnit actual; // 实际
    QString jsonPacket;
    QString datetime;
    QString mac,sn;
    QString ver;
    QVariantList mcutemp;
    int alarm;
};



class Core_Object : public QThread
{
    Q_OBJECT
public:
    explicit Core_Object(QObject *parent = nullptr);
    static sCoreItem coreItem;
    void clearAllEle();
    void clearLogs();
    void enCascade(int v);
    void factoryRestore();
    void rtuSet(int v);
    void boxSet(int v);
    void relayCtrl(int on, int id=0);
    void relayDelay(int sec, int id=0);
    void readMetaData();
    void timeSync();
    void irqCheck();
    void setRunTime();
    bool jsonAnalysis();

private:
    void getSn(const QJsonObject &object);
    void getMac(const QJsonObject &object);
    void getOutputVol(const QJsonObject &object);
    void getParameter(const QJsonObject &object);
    void getThreshold(const QJsonObject &object);
    void getAlarmStatus(const QJsonObject &object);
    double getRating(const QJsonObject &object, const QString &key, const QString &suffix="rated");

    bool checkInput(const QByteArray &msg, QJsonObject &obj);
    double getData(const QJsonObject &object, const QString &key);
    QJsonValue getValue(const QJsonObject &object, const QString &key);
    QJsonArray getArray(const QJsonObject &object, const QString &key);
    QJsonObject getObject(const QJsonObject &object, const QString &key);

protected:
    Core_Http *mHttp;
};

#endif // CORE_OBJECT_H
