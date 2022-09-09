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
    bool ret = false;
    QString md5 = readCfg("md5", "", fn).toString();    
    if(md5.size()) ret = md5 == File::md5(mDir + "/"+fn);
    return ret;
}

bool Cfg_App::app_pack(sAppVerIt &it)
{
    QString fn = mDir + "/" + it.app;
    it.releaseDate = QDate::currentDate().toString("yyyy-MM-dd");
    it.md5 = File::md5(fn);

    writeCfg("usr", it.usr, it.app);
    writeCfg("md5", it.md5, it.app);
    writeCfg("ver", it.ver, it.app);
    writeCfg("remark", it.remark, it.app);
    writeCfg("oldVersion", it.oldVersion, it.app);
    writeCfg("releaseDate", it.releaseDate, it.app);

    return it.md5.size();
}

bool Cfg_App::app_unpack(sAppVerIt &it)
{
    QString fn = mDir + "/" + it.app;
    it.usr = readCfg("usr", "", it.app).toString();
    it.md5 = readCfg("md5", "", it.app).toString();
    it.ver = readCfg("ver", "", it.app).toString();
    it.remark = readCfg("remark", "", it.app).toString();
    it.oldVersion = readCfg("oldVersion", "", it.app).toString();
    it.releaseDate = readCfg("releaseDate", "", it.app).toString();

    return app_check(it.app);
}
