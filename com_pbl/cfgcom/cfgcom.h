#ifndef CFGCOM_H
#define CFGCOM_H
#include "cfgserial.h"

#define MINTOR_NUMER 5

struct sCfgComIt
{
    QString user;
    sSerial coms;
    int logCount;
    uchar pcNum;

    uchar addr;
    uchar step; // 步骤
    ushort Max[MINTOR_NUMER];//电压-电流-功率
    ushort Min[MINTOR_NUMER];
    uchar recvaddr[MINTOR_NUMER];
    uchar recvfn[MINTOR_NUMER];
    uchar recvlen[MINTOR_NUMER];
    ushort data[MINTOR_NUMER];
    QString noldata[MINTOR_NUMER];
    QString nolmin[MINTOR_NUMER];
    QString nolmax[MINTOR_NUMER];
    ushort volErr, curErr, powErr; // 电流误差
    uchar result;

    int cnt;
    int all;
    int ok;
    int err;
};


/**
 * RTU传输统计结构体
 */

class CfgCom : public CfgSerial
{
    CfgCom(const QString& fn, QObject *parent);
public:
    static CfgCom* bulid(const QString& fn=CFG_FN, QObject *parent = nullptr);
    sCfgComIt *item;
    void writeCfgCom();
    void initMaxMinData();
    void readMaxMinData();
private:
    void initCfgCom();
};

#endif // CFGCOM_H
