#ifndef CONFIGBASH
#define CONFIGBASH
#include "cfgserial.h"


enum eDevTypes {
    IMM=0,

    AC = 1, // 交流
    DC,     // 直流

    Sum=0,
    Transformer, // 互感器
    Mn    // 锰铜
};


/**
 * RTU传输统计结构体
 */
struct sCount
{
    int cnt;
    int all;
    int ok;
    int err;
};

struct sErrData
{
    ushort vol;
    ushort volErr;
    ushort curErr;
    ushort powErr; // 电流误差
};

struct sCfgItem
{
    QString user;
    sErrData errs;
    sSerial coms;
    sCount cnts;

    int logCount;
    uchar aiMode;
    uchar pcNum;
};


class Cfg : public CfgSerial
{
    Cfg(QObject *parent = nullptr);
public:
    static Cfg *bulid(QObject *parent = nullptr);

    sCfgItem *item;
    void writeCnt();
    void writeCfgDev();
    void writeErrData();

protected:
    void initCnt();
    void initCfgDev();
    void initErrData();
};

#endif // CONFIGBASH
