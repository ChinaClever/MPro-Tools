/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"
#include <QImage>
#include <QPixmap>
#include "dbmacs.h"

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    mLogo = "usr/data/pdu/cfg/logo.png";
    mTls = "usr/data/pdu/certs/client-cert.pem";
    QString dir = "usr/data/pdu/cfg";
    FileMgr::build().mkpath(dir);

    dir = "usr/data/pdu/certs";
    FileMgr::build().mkpath(dir);
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
        QStringList ips = ssdp->searchAll();
        if(ips.isEmpty()) ips = ssdp->searchAll();
        QString str = tr("未找到任何目标设备"); // cm_mdelay(150);
        if(ips.size()) str = tr("已找到%1个设备").arg(ips.size());
        else {ret = false;} m_ips = ips;
        emit msgSig(str, ret,"","",""); //cout << ips;
    }
    return ret;
}


bool Core_Thread::timeCheck()
{
    bool ret = false;
    QString str = tr("设备时间，");
    QString strEn = tr("Equipment time，");

    QString fmd = "yyyy-MM-dd hh:mm:ss";
    QDateTime dt = QDateTime::fromString(coreItem.datetime, fmd);
    QDateTime time = QDateTime::currentDateTime();
    int secs = qAbs(time.secsTo(dt));
    if(secs > 600 && coreItem.timeCheck) {
        str += tr("相差过大：");
        strEn += tr("The difference is too large:");
    }else ret = true;
    str += coreItem.datetime;
    strEn += coreItem.datetime;

    emit msgSig(str, ret,"时间符合要求","参数检查","时间检查");
    emit msgSigEn(strEn, ret,"Time meets requirements", "Parameter check", "Time check" );
    return ret;
}

bool Core_Thread::snCheck()
{
    bool ret = false;
    QString sn = coreItem.sn;
    QString str = tr("序列号，");
    QString strEn = tr("SN,");


    if(sn.contains("2I3")) {str += sn; strEn += sn; ret = true;}
    else {str += tr("错误：sn=%1").arg(sn); strEn += tr("error：sn=%1").arg(sn);}

    QString uuid = coreItem.uuid;
    emit msgSig(str, ret, "序列号不重复","参数检查","序列号检查");
    emit msgSigEn(strEn, ret,"SN unique", "Parameter check", "SN check");

    if(ret) {
        if(mHashSn.contains(sn)) {
            if(mHashSn.value(sn) != uuid) {
                ret = false;
                emit msgSig(tr("SN：%1已被分配给UUID:%2").arg(sn, uuid), ret, "序列号不重复","参数检查","序列号检查");
                emit msgSigEn(tr("SN:%1 has been assigned to UUID:%2").arg(sn, uuid), ret, "SN unique", "Parameter check", "SN check");
            }
        } else mHashSn[sn] = uuid;
    }

    return ret;
}

bool Core_Thread::mcuTempCheck()
{
    bool res = true; int maxTemp = 65;
    int cnt = coreItem.actual.param.boardNum;
    int outputNum = coreItem.actual.param.outputNum;
    if(outputNum > 26) maxTemp += 20;
    QVariantList list = coreItem.mcutemp;
    for(int i=0; i<cnt && i<list.size(); ++i) {
        int temp = list.at(i).toInt();
        bool ret = true; if(!temp) continue;
        QString str = tr("第%1块执行板温度%2°c").arg(i+1).arg(temp);
        QString strEn = tr("Temperature of the %1st execution board is %2°C").arg(i+1).arg(temp);
        if(temp > maxTemp || temp < 5){ res = ret = false; str += tr("错误"),strEn += tr("error");}
        emit msgSig(str, ret,"执行板温度在期望值范围内","参数检查","执行板温度检查"); //cout << str;
        emit msgSigEn(strEn, ret,"Execution board temperature is within the expected range","Parameter check","Execution board temperature check");
    }

    return res;
}

bool Core_Thread::macCheck()
{   
    QString v = coreItem.mac;  bool ret = false;
    QString request,requestEn;
    QString str = tr("MAC地址，");
    QString strEn = tr("MAC addr,");

    if(coreItem.checkMac) {
        request = "MAC地址前缀为 2C:26:";
        requestEn = "MAC address prefix 2C:26:";
        if(v.contains("2C:26:")) {str += v; strEn += v; ret = true; }
        else {str += tr("错误：mac=%1").arg(v); strEn += tr("error: mac = %1");}
    } else {ret = true; str += tr("跳过检查：mac=%1").arg(v); strEn += tr("Skip the check: Mac = %1").arg(v); request = "客户定制MAC地址";}

    emit msgSig(str, ret,request,"参数检查","MAC地址检查");
    emit msgSigEn(strEn,ret,requestEn,"Parameter check","MAC address check");

    QString uuid = coreItem.uuid;
    QString sn = coreItem.sn; if(ret) {
        if(mHashMac.contains(v)) {
            if(mHashMac.value(v) != uuid) { ret = false;
                emit msgSig(tr("MAC：%1已被分配给UUID:%2 ")
                                .arg(v, mHashMac.value(v)), ret,"uuid不重复","参数检查","uuid检查");
                emit msgSigEn(tr("MAC：%1 has been assigned to UUID:%2 ")
                                  .arg(v, mHashMac.value(v)),ret,"UUIDs must be unique.","Parameter check","UUID check");
            }
        } else {
            int rtn = DbLogs::bulid()->contains(v, sn);
            if(rtn) {
                ret = false;
                emit msgSig(tr("MAC：%1已被分配, 在数据库已存在").arg(v), ret,"uuid不重复","参数检查","uuid检查");
                emit msgSigEn(tr("MAC：%1 has already been allocated and already exists in the database.").arg(v),
                              ret,"UUIDs must be unique.","Parameter check","UUID check");
            }
            else mHashMac[v] = uuid;
        }
    }

    return ret;
}

bool Core_Thread::supplyVolCheck()
{
    sParameter *actual = &coreItem.actual.param;
    bool ret = true; uint vol = actual->supplyVol;
    QString str = tr("12电源检查：vol = %1V").arg(vol / 1000.0);
    QString strEn = tr("12 Power supply check: vol = %1V").arg(vol / 1000.0);

    if(vol < 11*1000 || vol > 13*1000) {
        str += tr("异常");
        strEn += tr("abnormal");
        ret = false;
    }

    emit msgSig(str, ret,"电源符合规格","参数检查","电源检查");
    emit msgSigEn(strEn, ret,"Power supply conforms to specifications","Parameter check","Power supply check");

    return ret;
}

bool Core_Thread::downLogo(const QString &ip)
{
    QString str = tr("Logo图片下载：");
    QStringList fs; fs << mLogo;

    QFile::remove(mLogo);
    Core_Http *http = Core_Http::bulid(this);
    http->initHost(ip); http->downFile(fs);
    for(int i=0; i<1000; i+= 100) {
        if(QFile::exists(mLogo)) break; else cm_mdelay(100);
    }

    bool ret =QFile::exists(mLogo);
    if(ret) str += tr("成功"); else str += tr("失败");
    emit msgSig(str, ret,"","","");
    return ret;
}

// 计算两张图片的差异
bool Core_Thread::compareImages()
{
    if(File::fileSize(mLogo) != File::fileSize(coreItem.logo))  return false;

    // 加载两个图片文件
    QImage img1(mLogo);
    QImage img2(coreItem.logo);

    // 将两张图片转换为QImage类型，以便进行像素比较
    //QImage img1 = pix1.toImage();
    //QImage img2 = pix2.toImage();

    // 检查图片尺寸是否相同
    if (img1.size() != img2.size()) {
        return false;
    }

    // 计算不同像素点的数量
    uint diffCount = 0; uint count = 0;
    for (int y = 0; y < img1.height(); ++y) {
        for (int x = 0; x < img1.width(); ++x) {
            if (img1.pixelColor(x, y) != img2.pixelColor(x, y)) {
                ++diffCount;
            } ++count;
        }
    }

    // 设置一个阈值，判断两张图片是否大致相同
    const double threshold = 0.1; // 根据实际情况调整阈值
    if ((diffCount*1.0)/count <= threshold) {
        return true;
    } else {
        return false;
    }
}


bool Core_Thread::downTlsCert(const QString &ip)
{
    QString str = tr("Tls证书下载：");
    QStringList fs; fs << mTls;

    QFile::remove(mTls);
    Core_Http *http = Core_Http::bulid(this);
    http->initHost(ip); http->downFile(fs);
    for(int i=0; i<1000; i+= 100) {
        if(QFile::exists(mTls)) break; else cm_mdelay(100);
    }

    bool ret = QFile::exists(mTls);
    if(ret) str += tr("成功"); else str += tr("失败");
    emit msgSig(str, ret,"","","");
    return ret;
}

bool Core_Thread::compareTls()
{
    if(File::fileSize(mTls) != File::fileSize(coreItem.tlsFile))  return false;
    return File::md5(mTls) == File::md5(coreItem.tlsFile);
}

bool Core_Thread::tlsCertCheck(const QString &ip)
{
    bool ret = true;
    if(coreItem.desire.param.standNeutral) {
        if(coreItem.tlsFile.size()) {
            ret = downTlsCert(ip);
            if(ret) {
                ret = compareTls();
                QString str = tr("证书差异检查：");
                QString strEn = tr("Certificate difference check:");

                if(ret) str += tr("通过"),strEn += tr("Pass"); else str += tr("失败"),strEn += tr("Fail");

                emit msgSig(str, ret,"证书和期望一致" ,"参数检查","证书检查");
                emit msgSigEn(strEn, ret,"Certificate matches expectation", "Parameter check", "Certificate check");
            }

        } else {
            emit msgSig(tr("未指定证书此项检测跳过"), true,"证书和期望一致" ,"参数检查","证书检查");
            emit msgSigEn(tr("No certificate specified, skip this check"), true,"Certificate matches expectation","Parameter check","Certificate check");
        }
    }

    return ret;

}

bool Core_Thread::logoCheck(const QString &ip)
{
    bool ret = true;
    if(coreItem.desire.param.standNeutral<2) {
        emit msgSig(tr("标准和中性版本跳过Logo检测"), true,"Logo和期望值一致" ,"参数检查","Logo检查");
        emit msgSigEn(tr("Standard and neutral versions skip logo detection"), true,"Logo matches expectation", "Parameter check", "Logo check");
    } else if (coreItem.logo.isEmpty()) {
        emit msgSig(tr("未指定Logo此项检测跳过"), true,"Logo和期望值一致" ,"参数检查","Logo检查");
        emit msgSigEn(tr("Logo not specified, skip this check"), true,"Logo matches the expected value", "Parameter check", "Logo check");
    } else {
        ret = downLogo(ip);
        if(ret) {
            ret = compareImages();
            QString str = tr("Logo图片差异检查：");
            QString strEn = tr("Logo image difference check:");
            if(ret) str += tr("通过"),strEn += tr("Pass"); else str += tr("失败"),strEn += tr("Fail");
            emit msgSig(str, ret,"Logo和期望值一致" ,"参数检查","Logo检查");
            emit msgSigEn(strEn, ret,"Logo matches expected value", "Parameter check", "Logo check");
        }
    }

    return ret;
}

bool Core_Thread::parameterCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;

    QString str = tr("规格：");
    QString strEn = tr("Specification:");


    bool ret=true, res=true;
    if(desire->devSpec == actual->devSpec) {
        str += tr("%1系列").arg((char)(desire->devSpec+'A'-1));
        strEn += tr("%1 series").arg((char)(desire->devSpec+'A'-1));

    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->devSpec).arg(actual->devSpec);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->devSpec).arg(actual->devSpec);

    }

    emit msgSig(str, ret,"规格和期望值一致" ,"参数检查","规格检查");
    emit msgSigEn(strEn, ret,"Specifications and expectations are consistent" ,"Parameter check","Specifications inspection");


    str = tr("语言：");
    strEn = tr("language:");
    ret = true;
    if(desire->language == actual->language) {
        if(desire->language) str += tr("英文"),strEn += tr("English");

        else str += tr("中文"), strEn += tr("Chinese");
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->language).arg(actual->language);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->language).arg(actual->language);

    }

    emit msgSig(str, ret,"语言和期望值一致","参数检查","语言检查");
    emit msgSigEn(strEn, ret,"Language and expectations are consistent","Parameter check","Language check");

    str = tr("断路器：");
    strEn = tr("breaker:");

    ret = true;
    if(desire->isBreaker == actual->isBreaker) {
        if(desire->isBreaker) str += tr("存在"),strEn += tr("exist");
        else str += tr("没有"),strEn += tr("Does not exist");
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->isBreaker).arg(actual->isBreaker);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->isBreaker).arg(actual->isBreaker);

    }

    emit msgSig(str, ret,"断路器和期望值一致","参数检查","断路器检查");
    emit msgSigEn(strEn, ret,"The circuit breaker matches the expected value.","Parameter check","Circuit breaker inspection");

    str = tr("屏幕：");
    strEn = tr("Screen:");
    ret = true;
    if(desire->vh == actual->vh) {
        if(desire->vh) str += tr("水平"),strEn += tr("horizontal");
        else str += tr("垂直"), strEn += tr("vertical");
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->vh).arg(actual->vh);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->vh).arg(actual->vh);

    }
    emit msgSig(str, ret,"屏幕方向和期望值一致","参数检查","屏幕方向检查");
    emit msgSigEn(strEn, ret,"The screen orientation matches the expected value.","Parameter check","Screen orientation check");

    str = tr("版本：");
    strEn = tr("Version:");
    ret = true;
    if(desire->standNeutral == actual->standNeutral) {
        if(desire->standNeutral==0) str += tr("标准"),strEn += tr("standard");
        else if(desire->standNeutral==1) str += tr("中性"),strEn += tr("neutral");
        else str += tr("定制"),strEn += tr("custom");
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->standNeutral).arg(actual->standNeutral);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->standNeutral).arg(actual->standNeutral);

    }

    emit msgSig(str, ret,"版本和期望值一致","参数检查","版本检查");
    emit msgSigEn(strEn, ret,"Version matches expected value","Parameter check","Version check");

    str = tr("协议：");
    strEn = tr("Protocol:");
    ret = true;
    if(desire->oldProtocol == actual->oldProtocol) {
        if(desire->oldProtocol) str += tr("定制"),strEn += tr("custom"); else str += tr("标准"),strEn += tr("standard");
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->oldProtocol).arg(actual->oldProtocol);
        strEn += tr("expected value%1, actual value%2").arg(desire->oldProtocol).arg(actual->oldProtocol);
    }

    emit msgSig(str, ret,"协议和期望值一致" ,"参数检查","协议检查");
    emit msgSigEn(strEn, ret,"Protocol and expected values ​​match", "Parameter check", "Protocol check");

    str = tr("网页背景色：");
    strEn = tr("Web page background color:");
    ret = true;
    if(desire->webBackground == actual->webBackground) {
        if(desire->webBackground) str += tr("定制"),strEn += tr("custom"); else str += tr("标准-蓝色"),strEn += tr("Standard-Blue");
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->webBackground).arg(actual->webBackground);
        strEn += tr("expected value%1, actual value%2").arg(desire->webBackground).arg(actual->webBackground);
    }

    emit msgSig(str, ret,"网页背景色和期望值一致","参数检查","网页背景色检查");
    emit msgSigEn(strEn, ret,"The webpage background color matches the expected value","Parameter check","Webpage background color check");

    //    str = tr("传感器盒子："); ret = true;
    //    if(desire->sensorBoxEn == actual->sensorBoxEn) {
    //        if(desire->sensorBoxEn) str += tr("存在"); else str += tr("不带");
    //    } else {
    //        res = ret = false; str += tr("期望值%1,实际值%2")
    //                   .arg(desire->sensorBoxEn).arg(actual->sensorBoxEn);
    //    } emit msgSig(str, ret,"传感器盒子和期望值一致","参数检查","传感器检查");

    return res;
}

bool Core_Thread::devNumCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;
    QString str = tr("设备相数：");
    QString strEn = tr("Number of phases in the device:");
    bool ret=true, res=true;

    if(desire->lineNum == actual->lineNum) {
        if(1==desire->lineNum) str += tr("单相"),strEn += tr("Single phase"); else str += tr("三相"), strEn += tr("Three-phase");
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->lineNum).arg(actual->lineNum);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->lineNum).arg(actual->lineNum);

    }

    emit msgSig(str, ret, "相数和期望值一致","参数检查","相数检查");
    emit msgSigEn(strEn, ret, "The number of phases is consistent with the expected value.","Parameter check","Phase count check");

    str = tr("执行板数量：");
    strEn = tr("Number of execution boards:");
    ret = true;
    if(desire->boardNum == actual->boardNum) {
        str += QString::number(desire->boardNum);
        strEn += QString::number(desire->boardNum);
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->boardNum).arg(actual->boardNum);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->boardNum).arg(actual->boardNum);

    }

    emit msgSig(str, ret,"执行版数量和期望值一致","参数检查","执行版数量检查");
    emit msgSigEn(strEn, ret, "The number of executed versions is consistent with the expected value.","Parameter check","Execution version quantity check");

    str = tr("回路数量：");
    strEn = tr("Number of loops:");
    ret = true;
    if(desire->loopNum == actual->loopNum) {
        str += QString::number(desire->loopNum);
        strEn += QString::number(desire->loopNum);
    } else {
        res = ret = false;

        str += tr("回路数量期望值%1,实际值%2").arg(desire->loopNum).arg(actual->loopNum);
        strEn += tr("loop nums Expected value %1, Actual value %2").arg(desire->loopNum).arg(actual->loopNum);

    }

    emit msgSig(str, ret,"回路数量和期望值一致","参数检查","回路数量检查");
    emit msgSigEn(strEn, ret,"The number of loops is consistent with the expected value.","Parameter check","Loop Quantity Check");

    str = tr("输出位数量：");
    strEn = tr("Number of output bits:");

    ret = true;
    if(desire->outputNum == actual->outputNum) {

        str += QString::number(desire->outputNum);

    } else {

        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->outputNum).arg(actual->outputNum);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->outputNum).arg(actual->outputNum);

    }

    emit msgSig(str, ret,"输出位数量和期望值一致","参数检查","输出位数量检查");
    emit msgSigEn(strEn, ret,"The number of output bits is consistent with the expected value.","Parameter check","Output bit count check");

    return res;
}

bool Core_Thread::fwCheck()
{
    sVersion *desire = &coreItem.desire.ver;
    sVersion *actual = &coreItem.actual.ver;
    bool ret=true, res=true;

    QString str = tr("软件版本号：");
    QString strEn = tr("Fw Version:");
    ret = true;
    if(desire->fwVer == actual->fwVer) {
        str += desire->fwVer;
        strEn += desire->fwVer;
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->fwVer, actual->fwVer);
        strEn += tr("expected value%1, actual value%2").arg(desire->fwVer, actual->fwVer);
    }

    emit msgSig(str, ret, "软件版本号和期望值一致","参数检查","软件版本号检查");
    emit msgSigEn(strEn, ret, "The software version number matches the expected value.","Parameter check","Software version number check");

    str = tr("设备类型：");
    strEn = tr("Device type:");

    ret = true;
    if(desire->devType == actual->devType) {
        str += desire->devType;
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->devType, actual->devType);
        strEn += tr("expected value%1, actual value%2").arg(desire->devType, actual->devType);
    }
    emit msgSig(str, ret,"设备类型和期望值一致","参数检查","设备类型检查");
    emit msgSigEn(strEn, ret,"The equipment type matches the expected value.","Parameter check","Equipment type check");

    int num = coreItem.actual.param.loopNum;
    for(int i=0; i<num; ++i) {
        str = tr("第%1回路的输出位数量: ");
        strEn = tr("Number of output bits for the %1st loop: ");
        ret = true;
        int a = desire->loopOutlets.at(i).toInt();
        int b = actual->loopOutlets.at(i).toInt();
        if(a == b) {
            str += QString::number(b);
            strEn += QString::number(b);
        } else {
            res = ret = false;
            str += tr("期望值%1,实际值%2") .arg(a).arg(b);
            strEn += tr("expected value%1, actual value%2") .arg(a).arg(b);
        }
        emit msgSig(str.arg(i+1), ret,"回路输出位数量和期望值一致","参数检查","回路输出位数量检查");
        emit msgSigEn(strEn.arg(i+1), ret,"The number of output bits in the loop is consistent with the expected value.","Parameter check","Loop output bit count check");
    }

    return res;
}

bool Core_Thread::thresholdCheck()
{
    sThreshold *desire = &coreItem.desire.rate;
    sThreshold *actual = &coreItem.actual.rate;
    QString str = tr("相额定电压：");
    QString strEn = tr("Phase rated voltage: ");

    bool ret=true, res=true;
    if(desire->lineVol == actual->lineVol) {
        str += QString::number(desire->lineVol) +"V";
        strEn += QString::number(desire->lineVol) +"V";
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->lineVol).arg(actual->lineVol);
        strEn += tr("expected value%1, actual value%2").arg(desire->lineVol).arg(actual->lineVol);
    }

    emit msgSig(str, ret,"相额定电压和期望值一致","参数检查","相额定电压检查");
    emit msgSigEn(strEn, ret,"Phase rated voltage matches expected value","Parameter check","Phase rated voltage check");

    str = tr("相额定电流：");
    strEn = tr("Phase rated current: ");
    ret = true;

    if(desire->lineCur == actual->lineCur) {
        str += QString::number(desire->lineCur) +"A";
        strEn += QString::number(desire->lineCur) +"A";
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->lineCur).arg(actual->lineCur);
        strEn += tr("Expected value %1, Actual value %2").arg(desire->lineCur).arg(actual->lineCur);

    }

    emit msgSig(str, ret,"相额定电流期望值一致","参数检查","相额定电流检查");
    emit msgSigEn(strEn, ret,"Phase rated current expected value consistent","Parameter check","Phase rated current check");

    str = tr("相额定功率：");
    strEn = tr("Phase rated power: ");
    ret = true;
    if(desire->linePow == actual->linePow) {
        str += QString::number(desire->linePow) +"KW";
        strEn += QString::number(desire->linePow) +"KW";
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->linePow).arg(actual->linePow);
        strEn += tr("expected value%1, actual value%2").arg(desire->linePow).arg(actual->linePow);
    }

    emit msgSig(str, ret,"相额定功率期望值一致","参数检查","相额定功率检查");
    emit msgSigEn(strEn, ret,"Phase rated power expected value consistent","Parameter check","Phase rated power check");

    str = tr("回路额定电压：");
    strEn = tr("Rated circuit voltage: ");
    ret = true;
    if(desire->loopVol == actual->loopVol) {
        str += QString::number(desire->loopVol) +"V";
        strEn += QString::number(desire->loopVol) +"V";
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->loopVol).arg(actual->loopVol);
        strEn += tr("expected value%1, actual value%2").arg(desire->loopVol).arg(actual->loopVol);
    }

    emit msgSig(str, ret,"回路额定电压和期望值一致","参数检查","回路额定电压检查");
    emit msgSigEn(strEn, ret,"Circuit rated voltage matches expected value","Parameter check","Circuit rated voltage check");

    str = tr("回路额定电流：");
    strEn = tr("Rated current of the circuit: ");
    ret = true;
    if(desire->loopCur == actual->loopCur) {
        str += QString::number(desire->loopCur) +"A";
        strEn += QString::number(desire->loopCur) +"A";
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->loopCur).arg(actual->loopCur);
        strEn += tr("expected value%1, actual value%2").arg(desire->loopCur).arg(actual->loopCur);
    }

    emit msgSig(str, ret,"回路额定电流和期望值一致","参数检查","回路额定电流检查");
    emit msgSigEn(strEn, ret,"Circuit rated current matches expected value","Parameter check","Circuit rated current check");

    str = tr("回路额定功率：");
    strEn = tr("Rated power of the circuit: ");

    ret = true;
    if(desire->loopPow == actual->loopPow) {
        str += QString::number(desire->loopPow) +"KW";
        strEn += QString::number(desire->loopPow) +"KW";
    } else {
        res = ret = false;
        str += tr("期望值%1,实际值%2").arg(desire->loopPow).arg(actual->loopPow);
        strEn += tr("expected value%1, actual value%2").arg(desire->loopPow).arg(actual->loopPow);
    }
    emit msgSig(str, ret,"回路额定功率和期望值一致","参数检查","回路额定功率检查");
    emit msgSigEn(strEn, ret,"Circuit rated power matches expected value","Parameter check","Circuit rated power check");
    return res;
}


bool Core_Thread::outletCheck()
{
    QVariantList tmpList; bool ret = true; QString str,strEn;
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
            strEn = tr("Output bit %1 rated current:").arg(i+1);

            ret = false;
            str += tr("期望值%1A,实际值%2A").arg(d).arg(s);
            strEn += tr("Expected value%1A, Actual value%2A").arg(d).arg(s);
            emit msgSig(str, ret,"输出位额定电流和期望值一致","参数检查","输出位额定电流检查");
            emit msgSigEn(strEn, ret,"Output bit rated current matches expected value","Parameter check","Output bit rated current check");
        }
    }

    if(ret) {
        str = tr("输出位额定电流检查：OK");
        strEn = tr("Output bit rated current check: OK");
        emit msgSig(str, ret ,"输出位额定电流和期望值一致","参数检查","输出位额定电流检查");
        emit msgSigEn(strEn, ret ,"output bit rated current matches expected value","parameter check","output bit rated current check");
    }

    return ret;
}

bool Core_Thread::outputVolCheck()
{
    bool ret = true;  QString str,strEn;
    int spec = coreItem.actual.param.devSpec;
    int num = coreItem.actual.param.outputNum;
    int value = coreItem.actual.rate.volValue * 10.0;
    int lineNum = coreItem.actual.param.lineNum;
    QVariantList vols = coreItem.actual.rate.outputVols;

    if(spec > 1) {
        int end = num / lineNum;
        for(int k = 0; k<lineNum; ++k) {
            int vol = coreItem.actual.rate.lineVolValues.at(k).toDouble() * 10.0;
            for(int i=0; i<vols.size()&&(i<end); ++i) {
                int v = vols.at(i+k*end).toInt();
                int s = qAbs(v - vol); //cout << v << s;
                if(spec == 3 && !v){ continue; } if(s > 50 || !v) {
                    str = tr("输出位%1电压相差过大：相电压：%2V 输出位电压：%3V").arg(i+k*end+1).arg(vol/10.0).arg(v/10.0);
                    strEn = tr("Output bit %1 voltage difference is too large: Phase voltage: %2V Output bit voltage: %3V").arg(i+k*end+1).arg(vol/10.0).arg(v/10.0);
                    ret = false;
                    emit msgSig(str, ret,"输出位电压和期望值在误差范围内","参数检查","输出位电压检查");
                    emit msgSigEn(strEn, ret,"Output bit voltage and expected value are within error range","Parameter check","Output bit voltage check");
                }
            }
        }

        // for(int i=0; i<vols.size()&&(i<num); ++i) {
        //     int v = vols.at(i).toInt();
        //     int s = qAbs(v - value); //cout << v << s;
        //     if(spec == 3 && !v){ continue; } if(s > 50 || !v) {
        //         str = tr("输出位%1电压相差过大：相电压：%2V 输出位电压：%3V")
        //                   .arg(i+1).arg(value/10.0).arg(v/10.0);
        //         ret = false;  emit msgSig(str, ret);
        //     }
        // }

        for(int i=1; i<vols.size()&&(i<num); ++i) {
            int v = vols.at(i).toInt();
            value = vols.at(i-1).toInt();
            int s = qAbs(v - value)/10.0; //cout << v << s;
            if(spec == 3 && !v){ continue; } if(s > 5 || !v) {
                str = tr("输出位%1(%2V) 与输出位%3(%4V) 电压相差过大")
                          .arg(i).arg(value/10.0).arg(i+1).arg(v/10.0);
                strEn = tr("The voltage difference between output bit %1 (%2V) and output bit %3 (%4V) is too large")
                          .arg(i).arg(value/10.0).arg(i+1).arg(v/10.0);
                ret = false;
                emit msgSig(str, ret,"两两输出位电压的差值在误差范围内","参数检查","输出位电压差值检查");
                emit msgSigEn(strEn, ret,"The difference between each pair of output bit voltages is within the error range","Parameter check","Output bit voltage difference check");
            }
        }
    }

    if(ret) {
        str = tr("输出位电压检查：OK");
        strEn = tr("Output bit voltage check: OK");

        emit msgSig(str, ret,"输出位电压误差在期望值范围内","参数检查","输出位电压检查");
        emit msgSigEn(strEn, ret,"Output bit voltage error is within expected range","Parameter check","Output bit voltage check");
    }

    return ret;
}

bool Core_Thread::alarmCheck()
{
    bool ret = false; if(coreItem.alarm) {
        QString str = tr("设备有报警，请检查：");
        QString strEn = tr("The device has an alarm, please check:");
        str += "status=" + QString::number(coreItem.alarm);
        strEn += "status=" + QString::number(coreItem.alarm);
        emit msgSig(str, ret,"设备没有告警信息","参数检查","告警检查");
        emit msgSigEn(strEn, ret,"The device has no alarm information.","Parameter check","Alarm Check");
    } else ret = true;
    return ret;
}

bool Core_Thread::tgCheck()
{
    bool res = true; bool ret = true;
    sMonitorData *it = &coreItem.actual.data;
    QString str = tr("设备总功率：%1kva").arg(it->apparent_pow);
    QString strEn = tr("Total device power: %1kva").arg(it->apparent_pow);
    if(it->apparent_pow >1) {res = ret = false;  str += tr("过大");}

    emit msgSig(str, ret,"设备总功率在期望值范围内" ,"参数检查","设备总功率检查");
    emit msgSigEn(strEn, ret,"Total device power within expected range", "Parameter check", "Total device power check");

    str = tr("设备总电能：%1kwh").arg(it->tg_ele);
    strEn = tr("Total power of equipment: %1kwh").arg(it->tg_ele);
    ret = true;
    if(it->tg_ele > 10) {res = ret = false;  str += tr("过大"),strEn += tr("Too large");}

    emit msgSig(str, ret,"设备总电能在期望值范围内" ,"参数检查","设备总功率检查");
    emit msgSigEn(strEn, ret,"Total power of the device is within the expected range", "Parameter check", "Total power of the device");
    return res;
}

bool Core_Thread::envCheck()
{
    bool res = true; bool ret = true;
    sMonitorData *it = &coreItem.actual.data;

    for(int i=0; i<4 && i< it->temps.size(); ++i) {
        double value = it->temps.at(i).toDouble();
        QString str = tr("传感器温度%1：%2°C").arg(i+1).arg(value);
        if(value < 5 || value > 50) {
            ret = res = false; str += tr("错误; ");
            if(i > 1) str += tr("检查传感器盒子连接状态");
        }
        emit msgSig(str, ret,"传感器温度在期望值范围内" ,"参数检查","传感器温度检查");
    }

    for(int i=0; i<2 && i< it->doors.size(); ++i) {
        int value = it->doors.at(i).toInt();
        QString str = tr("门禁%1：%2").arg(i+1).arg(value);
        if(0 == value) {ret = res = false; str += tr("错误");}
        emit msgSig(str, ret,"门禁状态和期望值一致" ,"参数检查","门禁检查");
    }

    return res;
}


bool Core_Thread::workDown(const QString &ip)
{
    bool res = true, ret;
    emit msgSig(tr("目标设备:")+ip, true,"","","");
    Core_Http *http = Core_Http::bulid(this);
    http->initHost(ip); readMetaData(); jsonAnalysis();
    ret = timeCheck(); if(!ret) res = false;
    ret = snCheck(); if(!ret) res = false;
    ret = fwCheck(); if(!ret) res = false;
    ret = macCheck(); if(!ret) res = false;
    ret = alarmCheck(); if(!ret) res = false;
    ret = devNumCheck(); if(!ret) res = false;
    ret = parameterCheck(); if(!ret) res = false;
    ret = supplyVolCheck(); if(!ret) res = false;
    ret = thresholdCheck(); if(!ret) res = false;
    ret = outputVolCheck(); if(!ret) res = false;
    ret = mcuTempCheck(); if(!ret) res = false;
    ret = outletCheck(); if(!ret) res = false;
    ret = logoCheck(ip); if(!ret) res = false;
    ret = tlsCertCheck(ip); if(!ret) res = false;
    // ret = envCheck(); if(!ret) res = false; /////// =======
    ret = tgCheck(); if(!ret) res = false;

    if(res) {
        enCascade(0); rtuSet(0); //clearAllEle()
        sParameter *desire = &coreItem.desire.param;
        if(0 == desire->sensorBoxEn) boxSet(0);
        if(coreItem.actual.param.devSpec > 2) {
            relayCtrl(1); relayDelay(1); //relayDelay(0);
        } emit msgSig("清除所有电能", true,"","",""); clearAllEle();
        emit msgSig("清除运行时间", true,"","",""); setRunTime();
        emit msgSig("清除设备日志", true,"","",""); clearLogs();

        cm_mdelay(1000);
        emit msgSig("恢复出厂设置", true,"","","");
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
            else emit msgSig(tr("目标设备不存在:")+ip, ret,"","","");
            emit finshSig(ret, ip+" ");
        } m_ips.clear();
    }  emit overSig();


    sCfgComIt *cfg = CfgCom::bulid()->item;

    if(!cfg->ipAddr.isEmpty()) {
        cm_mdelay(2*1000);
        Json_Pack *packer = Json_Pack::bulid();
        sProgress *pro = Json_Pack::bulid()->getPro();
        int num = pro->uploadPass.size();
        qDebug()<<"num: "<<num<<endl;;
 //       for(int i = 0 ; i < num; i ++ ){
            QJsonObject json,jsonEn;
            packer->head(json);
            packer->head(jsonEn);
            jsonEn.insert("languageSelect",1);
            Json_Pack::bulid()->http_post(cfg->meta, json, 0 , cfg->ipAddr);
            Json_Pack::bulid()->http_post(cfg->meta, jsonEn, 1 , cfg->ipAddr);

    }
}
