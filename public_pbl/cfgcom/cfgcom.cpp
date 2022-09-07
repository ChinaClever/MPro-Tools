/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "cfgcom.h"

CfgCom::CfgCom(QObject *parent)
{
    QCoreApplication::setOrganizationName("CLEVER");
    QCoreApplication::setOrganizationDomain("clever.com");
    QCoreApplication::setApplicationName("IMM-CalibrateTool");

    mCfgIni = nullptr;
    cfgOpen(parent);
}



/***
  * 获取程序数据目录
  */
QString CfgCom::pathOfData(const QString& name)
{
    QDir dataDir(QDir::home());  //QCoreApplication::applicationDirPath()
    QString dirName = "." + QCoreApplication::organizationName();
    if(!dataDir.exists(dirName)) {dataDir.mkdir(dirName);} dataDir.cd(dirName);

    dirName = QCoreApplication::applicationName();
    if(!dataDir.exists(dirName)) {dataDir.mkdir(dirName);} dataDir.cd(dirName);
    return dataDir.absoluteFilePath(name);
}

/**
 * 功 能：打开系统配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
bool CfgCom::cfgOpen(QObject *parent, const QString& fn)
{
    QString strFilename = pathOfData(fn);
    bool ret = QFileInfo::exists(strFilename);
    if(mCfgIni == nullptr) {
        mCfgIni = new QSettings(strFilename, QSettings::IniFormat, parent);
#if(QT_VERSION < QT_VERSION_CHECK(6,0,0))
        mCfgIni->setIniCodec(QTextCodec::codecForName("utf-8")); // gb18030
#endif
    }

    return ret;
}


/**
 * 功 能：参数写入配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
void CfgCom::write(const QString& key, const QVariant &v, const QString& g)
{
    mCfgIni->beginGroup(g);
    mCfgIni->setValue(key, v);
    mCfgIni->endGroup();
    //mCfgIni->sync();
}

/**
 * 功 能：读整形串配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
QVariant CfgCom::read(const QString &key, const QVariant &v, const QString& g)
{
    mCfgIni->beginGroup(g);
    QVariant ret = mCfgIni->value(key, v);
    mCfgIni->endGroup();

    return ret;
}
