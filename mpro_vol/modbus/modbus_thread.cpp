#include "modbus_thread.h"
#include <QByteArray>
Modbus_Thread::Modbus_Thread(QObject *parent) : QThread(parent)
{
    isRun = false;

    mItem = CfgCom::bulid()->item;
    mModbus = Ad_Modbus::bulid(this);
}

Modbus_Thread::~Modbus_Thread()
{
    if(isRun) {
        isRun = false;
        mItem->step = Test_Over;
    }
    wait();
}

Modbus_Thread *Modbus_Thread::bulid(QObject *parent)
{
    static Modbus_Thread* sington = nullptr;
    if(sington == nullptr)
        sington = new Modbus_Thread(parent);
    return sington;
}

void Modbus_Thread::infoRead(uchar addr,sRtuItem *pkt)
{
    pkt->fn = 0x03;
    if(addr == 1) pkt->reg = 0x000d;
    else pkt->reg = 0x0000;

    pkt->addr = 0x01 + addr -1;
    pkt->num = 1;
}
bool Modbus_Thread::getData(uchar addr)
{
    sRtuItem it;
    bool res = false;
    static uchar recv[MODBUS_RTU_SIZE] = {0};

    infoRead(addr,&it);
    int ret = mModbus->rtuRead(&it,recv,1);
    if(addr == 1) mItem->data[0] =(recv[0]*256 +recv[1]);
    else mItem->data[addr-1] =(recv[0]*256 +recv[1]);

    if(ret) res = true;
    memset(recv,0,sizeof(recv));

    return res;
}

bool Modbus_Thread::loopDevInfo(bool ret,int i)
{
    if(ret) {
        QString str;
        float temp;
        ret = getData(i);
        str = tr("读取电压表 %1 数据 ").arg(i);
        if(ret)str += tr("成功");else str += tr("失败");
        emit msgSig(str, ret);
        temp = mItem->data[i-1] / 1000.0;
        mItem->noldata[i-1] = QString::number(temp,'f',3);
        str = tr("电压表 %1 数据 vol= %2 V").arg(i).arg(mItem->noldata[i-1]);
        emit msgSig(str, ret);
    }

    return ret;
}
bool Modbus_Thread::readDevInfo()
{
    bool ret = false;
    float temp;
    ret = getData(1);
    QString str;
    str = tr("读取功率表 1 数据 ");
    if(ret)str += tr("成功");else str += tr("失败");
    emit msgSig(str, ret);
    temp = mItem->data[0] / 100.0;
    mItem->noldata[0] = QString::number(temp,'f',2);
    str = tr("功率表 1 数据 pow= %1 W").arg(mItem->noldata[0]);
    emit msgSig(str, ret);

    for(int i = 2;i<6;i++)
        if(mflag)ret = loopDevInfo(ret,i);
        else ret = false;

    return ret;

}
bool Modbus_Thread::compareData()
{
    bool ret = true;
    bool res = true ;
    QString str = tr("开始数据比较");
    emit msgSig(str, ret);
    str.clear();
    for(int i = 0;i < 5;i++)
    {
        if(mflag)
        {
            if(i == 0) mItem->data[0] = mItem->data[0] * 10;
            if((mItem->Min[i] <= mItem->data[i]) && (mItem->data[i] <= mItem->Max[i]))
            {
                //对比成功
                str += tr("表 %1 对比数据成功，最小值 %2，最大值 %3，实际值为 %4").arg(i+1).arg(mItem->Min[i] / 1000.0)
                           .arg(mItem->Max[i]/ 1000.0).arg(mItem->noldata[i]);
                ret = true;
            }else{
                //失败
                res = false;
                str += tr("表 %1 对比数据失败，最小值 %2，最大值 %3，实际值为 %4").arg(i+1).arg(mItem->Min[i] / 1000.0)
                           .arg(mItem->Max[i]/ 1000.0).arg(mItem->noldata[i]);
                ret = false;
            }
            emit msgSig(str, ret);
            str.clear();
        }else res = false;
    }

    return res;
}
void Modbus_Thread::workDown()
{
    bool ret = true;
    QString str = tr("开始读取数据：");
    emit msgSig(str, ret);
    if(mflag) ret = readDevInfo();
    else{ret = false;}

    if(!ret)
    {
        str.clear();
        str += tr("最终结果：");
        emit finshSig(ret, str);
        return;
    }

    if(mflag) ret = compareData();
    else{ret = false;}

    str.clear();
    str += tr("最终结果：");
    emit finshSig(ret, str);
}
void Modbus_Thread::run()
{
    if(!isRun) {
        isRun = true;
        workDown();
    }
    isRun = false; mItem->step = Test_End;
    emit overSig();
}
