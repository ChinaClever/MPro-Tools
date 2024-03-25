/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"
#include <QImage>
#include <QPixmap>

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    // mComWid = new SerialStatusWid();
    mSerial = new SerialPort();
    mItem = CfgCom::bulid()->item;
}

Core_Thread *Core_Thread::bulid(QObject *parent)
{
    static Core_Thread* sington = nullptr;
    if(!sington) sington = new Core_Thread(parent);
    return sington;
}

bool Core_Thread::timeCheck()
{
    bool ret = false;
    QString str = tr("设备时间，");
    QString fmd = "yyyy-MM-dd hh:mm:ss";
    QDateTime dt = QDateTime::fromString(coreItem.datetime, fmd);
    QDateTime time = QDateTime::currentDateTime();
    int secs = qAbs(time.secsTo(dt));
    if(secs > 10) str += tr("相差过大："); else ret = true;
    str += coreItem.datetime;
    emit msgSig(str, ret);
    return ret;
}
bool Core_Thread::snCheck()
{
    bool ret = false;bool res = false;
    int cnt = coreItem.actual.param.boardNum;
    QVariantList opVers = coreItem.actual.ver.opVers;
    QString str = tr("读取执行板版本号");
    QString str1 ;
    for(int i=0; i<cnt && i<opVers.size(); ++i) {
        QString version = opVers.at(i).toString();
        if(!version.isEmpty()){
            str1 = tr("第%1块执行板版本号为 %2").arg(i+1).arg(version);
            mItem->op_ver = version;
            res = true; ret = true; }
        else { ret = false; }
        emit msgSig(str1, ret); //cout << str;
    }
    if(ret) str += tr("成功");
    else str += tr("失败");
    emit msgSig(str, res);

    return res;
}
bool Core_Thread::envCheck()
{
    bool res = false; bool ret = false;
    sMonitorData *it = &coreItem.actual.data;
    for(int i=0; i<3 && i< it->temps.size(); i++) {
        double value = it->temps.at(i).toDouble();
        mItem->tem[i] = value;
        QString str = tr("传感器温度%1：%2°C").arg(i+1).arg(value);
        if(value <= 0) {
            ret = res = false; str += tr("错误; ");
            str += tr("检查传感器盒子连接状态");
        }else {ret = res = true;}
        emit msgSig(str, ret);
    }

    for(int i=0; i<2 && i< it->doors.size(); ++i) {
        int value = it->doors.at(i).toInt();
        mItem->doors[i] = value;
        QString str = tr("门禁%1：%2").arg(i+1).arg(value);
        if(value > 0) ret = true;else ret = false;
        emit msgSig(str, ret);
    }
    if(ret && res) res = true;    
    else res = false;

    return res;
}
bool Core_Thread::onserial(SerialPort *Item)
{
    bool ret = false;bool res = false;
    QByteArray array1,array2;
    emit msgSig(tr("开始检测"), true);
    QString str = tr("第1条指令发送");
    emit msgSig(str, true);
    QString str2 = tr("第2条指令发送");
    QString hexString1 = "0103000000104406";
    QString hexString2 = "0203000000104435";
    QByteArray byteArry1 = QByteArray::fromHex(hexString1.toLatin1());
    QByteArray byteArry2 = QByteArray::fromHex(hexString2.toLatin1());

    rtuSet(1);//开启modbus TRU
    sleep(2);
    if(Item->isOpened()){//第一次发送命令
        int len = Item->write(byteArry1);
        sleep(2);
        if(len){
            Item->read(array1,5);
            qDebug()<<"array.size1"<<array1.size();
            if(array1.size()){ret = true;str = tr("串口1接收数据成功！");}else{
                len = Item->write(byteArry1);
                if(len){
                    str = tr("再次发送第1条指令");
                    emit msgSig(str, true);
                    Item->read(array1,5);
                    qDebug()<<"array.size1"<<array1.size();
                    if(array1.size()){ret = true;str = tr("串口1接收数据成功！");
                    }else{ret = false;str = tr("串口1接收数据失败！");}
                }else{ret = false; str = tr("串口1接收数据失败！");}
            }
        }
        emit msgSig(str, ret);

        emit msgSig(str2, true);//第二次发送数据
        len = Item->write(byteArry2);sleep(3);
        if(len){
            Item->read(array2,5);
            qDebug()<<"array.size2"<<array2.size();
            if(array2.size()){res = true;str2 = tr("串口2接收数据成功！");}
            else{ res = false;str2 = tr("串口2接收数据失败！");}
        }
        emit msgSig(str2, res);
        sleep(2);
        rtuSet(0);//关闭modbus TRU
    }
    if(ret && res){ret = res = true;}
    else{ret = res = false;}

    return ret;
}
bool Core_Thread::workDown()
{
    bool res = false;
    bool ret = false;
    QString str = tr("传感器盒子检测 ");
    Core_Http *http = Core_Http::bulid(this);
    http->initHost();
    boxSet(1);//开启传感器盒子
    sleep(2);
    readMetaData();
    res = jsonAnalysis();

    if(res) ret = envCheck(); if(!ret) res = false;
    if(ret) str += tr("成功！");else str += tr("失败！");

    emit msgSig(str, ret);
    boxSet(0);//关闭传感器盒子

    return res;
}

void Core_Thread::run()
{
    bool ret =true; bool res =true;bool reg =true;
    if(ret) {
        QString str;
        res = onserial(mItem->coms.ser);
        if(res) str = tr("串口通讯成功！");
        else str = tr("串口通讯失败！");

        mItem->serial = str;
        emit msgSig(str, res);
        ret = workDown();//传感器盒子
        if(ret) {msgSig("网口通讯成功！", ret);mItem->netserial = "网口通讯成功！";}
        else {msgSig("网口通讯失败！", ret);mItem->netserial = "网口通讯失败！";}

        reg = snCheck();//执行板序列号读取
        if(ret && res && reg) res = ret = true; else ret = res = false;
        emit finshSig(ret,"最终结果");
    }  emit overSig();
}
