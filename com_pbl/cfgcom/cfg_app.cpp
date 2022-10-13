/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "cfg_app.h"

Cfg_App::Cfg_App(const QString& dir, QObject *parent)
    : Cfg_Obj(dir+"/"+CFG_APP, parent), mDir(dir)
{

}

bool Cfg_App::app_check(const QString &fn)
{
    bool ret = true; QString g = "app_pack";
    QString md5 = readCfg(fn, "", g).toString();
    if(md5.size()) ret = md5 == File::md5(mDir + "/"+fn);
    return ret;
}

void Cfg_App::app_dir(const QString &dir)
{
    QStringList fs = File::entryList(mDir+"/"+dir);
    foreach(const auto &app, fs) {
        if(app == "." || app == "..") continue;
        QString fn = mDir + "/" + dir + "/" + app;
        QString md5 = File::md5(fn);
        writeCfg(app.toUtf8(), md5, dir);
    }
}

bool Cfg_App::app_pack(sAppVerIt &it)
{
    QString g = "app_pack";
    app_dir("app");
    app_dir("bin");

    writeCfg("usr", it.usr, g);
    writeCfg("md5", it.md5, g);
    writeCfg("ver", it.ver, g);
    writeCfg("remark", it.remark, g);
    writeCfg("oldVersion", it.oldVersion, g);
    writeCfg("releaseDate", it.releaseDate, g);
    return it.md5.size();
}

bool Cfg_App::app_unpack(sAppVerIt &it)
{
    QString g = "app_pack";
    //it.apps = File::entryList(mDir+ "/app/");
    it.usr = readCfg("usr", "", g).toString();
    it.md5 = readCfg("md5", "", g).toString();
    it.ver = readCfg("ver", "", g).toString();
    it.remark = readCfg("remark", "", g).toString();
    it.oldVersion = readCfg("oldVersion", "", g).toString();
    it.releaseDate = readCfg("releaseDate", "", g).toString();
    QString str = it.usr + it.ver + it.remark + it.oldVersion + it.releaseDate;
    str = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5).toHex();
    return  it.md5 == str;
}
