#ifndef MODBUS_THREAD_H
#define MODBUS_THREAD_H
#include "modbus.h"

enum {
    Test_Fun, // 功能
    Test_Start, // 开始
    Test_Ading,
    Test_vert, // 验证
    Test_Over, // 终止
    Sendok,    //发送成功
    Sendfail,  //发送失败
    Test_End, // 完成
    Collect_Start, // 数据采集
    Test_Info=0,
    Test_Pass=1,
    Test_Fail=2,
};

class Modbus_Thread : public QThread
{
    Q_OBJECT
    explicit Modbus_Thread(QObject *parent = nullptr);
public:
    static Modbus_Thread *bulid(QObject *parent = nullptr);
    ~Modbus_Thread();

    void run();
    void workDown();
    bool delay(int s) {return mModbus->delay(s);}

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);
    void overSig();
private:
    bool isRun;
    sCfgComIt *mItem;
    Ad_Modbus *mModbus;
};
#endif // MODBUS_THREAD_H
