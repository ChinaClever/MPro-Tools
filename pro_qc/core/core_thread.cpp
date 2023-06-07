/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
}

Core_Thread *Core_Thread::bulid(QObject *parent)
{
    static Core_Thread* sington = nullptr;
    if(!sington) sington = new Core_Thread(parent);
    return sington;
}


bool Core_Thread::searchDev()
{
    bool ret = true; if(m_ips.isEmpty()) {
        Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
        cm_mdelay(15); QStringList ips = ssdp->searchAll();
        QString str = tr("未找到任何目标设备");
        if(ips.size()) str = tr("已找到%1个设备").arg(ips.size());
        else {ret = false;} m_ips = ips;
        emit msgSig(str, ret); //cout << ips;
    }
    return ret;
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
    bool ret = false;
    QString sn = coreItem.sn;
    QString str = tr("序列号，");
    if(sn.contains("2I3")) {str += sn; ret = true;}
    else {str += tr("错误：sn=%1").arg(sn);}
    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::macCheck()
{
    bool ret = false;
    QString v = coreItem.mac;
    QString str = tr("MAC地址，");
    if(v.contains("2C:26:")) {str += v; ret = true;}
    else {str += tr("错误：mac=%1").arg(v);}
    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::parameterCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;
    QString str = tr("规格："); bool ret=true, res=true;
    if(desire->devSpec == actual->devSpec) {
        str += tr("%1系列").arg((char)(desire->devSpec+'A'-1));
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                .arg(desire->devSpec).arg(actual->devSpec);
    } emit msgSig(str, ret);

    str = tr("语言："); ret = true;
    if(desire->language == actual->language) {
        if(desire->language) str += tr("英文"); else str += tr("中文");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->language).arg(actual->language);
    } emit msgSig(str, ret);

    str = tr("断路器："); ret = true;
    if(desire->isBreaker == actual->isBreaker) {
        if(desire->isBreaker) str += tr("存在"); else str += tr("没有");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->isBreaker).arg(actual->isBreaker);
    } emit msgSig(str, ret);

    str = tr("屏幕："); ret = true;
    if(desire->vh == actual->vh) {
        if(desire->vh) str += tr("水平"); else str += tr("垂直");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->vh).arg(actual->vh);
    } emit msgSig(str, ret);

    str = tr("中性："); ret = true;
    if(desire->standNeutral == actual->standNeutral) {
        if(desire->standNeutral) str += tr("中性"); else str += tr("标准");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->standNeutral).arg(actual->standNeutral);
    } emit msgSig(str, ret);

    str = tr("传感器盒子："); ret = true;
    if(desire->sensorBoxEn == actual->sensorBoxEn) {
        if(desire->sensorBoxEn) str += tr("存在"); else str += tr("不带");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->sensorBoxEn).arg(actual->sensorBoxEn);
    } emit msgSig(str, ret);

    return res;
}

bool Core_Thread::devNumCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;
    QString str = tr("设备相数：");
    bool ret=true, res=true;

    if(desire->lineNum == actual->lineNum) {
        if(1==desire->lineNum) str += tr("单相"); else str += tr("三相");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->lineNum).arg(actual->lineNum);
    } emit msgSig(str, ret);

    str = tr("执行板数量："); ret = true;
    if(desire->boardNum == actual->boardNum) {
        str += QString::number(desire->boardNum);
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->lineNum).arg(actual->lineNum);
    } emit msgSig(str, ret);

    str = tr("回路数量："); ret = true;
    if(desire->loopNum == actual->loopNum) {
        str += QString::number(desire->loopNum);
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->loopNum).arg(actual->loopNum);
    } emit msgSig(str, ret);

    str = tr("输出位数量："); ret = true;
    if(desire->outputNum == actual->outputNum) {
        str += QString::number(desire->outputNum);
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->outputNum).arg(actual->outputNum);
    } emit msgSig(str, ret);

    return res;
}


bool Core_Thread::thresholdCheck()
{
    sThreshold *desire = &coreItem.desire.rate;
    sThreshold *actual = &coreItem.actual.rate;
    QString str = tr("相额定电压：");
    bool ret=true, res=true;
    if(desire->lineVol == actual->lineVol) {
        str += QString::number(desire->lineVol) +"V";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->lineVol).arg(actual->lineVol);
    } emit msgSig(str, ret);

    str = tr("相额定电流："); ret = true;
    if(desire->lineCur == actual->lineCur) {
        str += QString::number(desire->lineCur) +"A";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->lineCur).arg(actual->lineCur);
    } emit msgSig(str, ret);

    str = tr("相额定功率："); ret = true;
    if(desire->linePow == actual->linePow) {
        str += QString::number(desire->linePow) +"KW";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->linePow).arg(actual->linePow);
    } emit msgSig(str, ret);


    str = tr("回路额定电压："); ret = true;
    if(desire->loopVol == actual->loopVol) {
        str += QString::number(desire->loopVol) +"V";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->loopVol).arg(actual->loopVol);
    } emit msgSig(str, ret);


    str = tr("回路额定电流："); ret = true;
    if(desire->loopCur == actual->loopCur) {
        str += QString::number(desire->loopCur) +"A";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->loopCur).arg(actual->loopCur);
    } emit msgSig(str, ret);

    str = tr("回路额定功率："); ret = true;
    if(desire->loopPow == actual->loopPow) {
        str += QString::number(desire->loopPow) +"KW";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->loopPow).arg(actual->loopPow);
    } emit msgSig(str, ret);

    return res;
}


bool Core_Thread::outletCheck()
{
    QVariantList tmpList; bool ret = true; QString str;
    QVariantList *desire = &coreItem.desire.rate.ops;
    QVariantList *actual = &coreItem.actual.rate.ops;
    for(int i=0; i<48; ++i) tmpList << 10;
    for(int i=0; i<desire->size(); ++i) {
        int id = desire->at(i).toInt();
        tmpList[id-1] = 16;
    }

    for(int i=0;i<actual->size(); ++i) {
        double d = tmpList.at(i).toInt();
        double s = actual->at(i).toInt();
        if(s != d) {
            str = tr("输出位%1额定电流：").arg(i+1);
            ret = false; str += tr("期望值%1A,实际值%2A")
                       .arg(d).arg(s);
             emit msgSig(str, ret);
        }
    }

    if(ret) {
        str = tr("输出位额定电流检查：OK");
        emit msgSig(str, ret);
    }

    return ret;
}



bool Core_Thread::workDown(const QString &ip)
{
    bool res = true, ret;
    emit msgSig(tr("目标设备:")+ip, true);
    Core_Http *http = Core_Http::bulid(this);
    http->initHost(ip); readMetaData(); jsonAnalysis();
    ret = timeCheck(); if(!ret) res = false;
    ret = snCheck(); if(!ret) res = false;
    ret = macCheck(); if(!ret) res = false;
    ret = parameterCheck(); if(!ret) res = false;
    ret = devNumCheck(); if(!ret) res = false;
    ret = outletCheck(); if(!ret) res = false;

    if(res) {
        emit msgSig("清除所有电能", true);
        clearAllEle(); cm_mdelay(1000);
        emit msgSig("恢复出厂设置", true);
        factoryRestore(); cm_mdelay(100);
    }

    return res;
}

void Core_Thread::run()
{
    bool ret = searchDev(); if(ret) {
        foreach (const auto &ip, m_ips) {
            ret = cm_pingNet(ip);
            if(ret) ret = workDown(ip);
            else emit msgSig(tr("目标设备不存在:")+ip, ret);
            emit finshSig(ret, ip+" ");
        }m_ips.clear();
    }  emit overSig();
}
