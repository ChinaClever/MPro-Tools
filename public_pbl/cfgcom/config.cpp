/*
 * 配置文件公共基类
 *
 *  Created on: 2018年1月1日
 *      Author: Lzy
 */
#include "config.h"

Cfg::Cfg(QObject *parent) : CfgSerial(parent)
{
    item = new sCfgItem();
    item->errs.vol = 200;

    initCnt();
    initCfgDev();
    initErrData();
}

Cfg *Cfg::bulid(QObject *parent)
{
    static Cfg* sington = nullptr;
    if(!sington) sington = new Cfg(parent);
    return sington;
}

void Cfg::initCfgDev()
{
    item->user = read("user", "", "User").toString();
    item->aiMode = read("ai_mode", 0, "Sys").toInt();
    item->pcNum = read("pc_num", 0, "Sys").toInt();
}

void Cfg::writeCfgDev()
{
    writeCnt();
    write("ai_mode", item->aiMode, "Sys");
    write("user", item->user, "User");
    write("pc_num", item->pcNum, "Sys");
}

void Cfg::initCnt()
{
    item->cnts.cnt = read("cnt", 0, "Cnts").toInt();
    item->cnts.all = read("all", 0, "Cnts").toInt();
    item->cnts.ok = read("ok", 0, "Cnts").toInt();
    item->cnts.err = read("err", 0, "Cnts").toInt();
}

void Cfg::writeCnt()
{
    write("cnt", item->cnts.cnt, "Cnts");
    write("all", item->cnts.all, "Cnts");
    write("ok", item->cnts.ok, "Cnts");
    write("err", item->cnts.err, "Cnts");
    write("user", item->user, "User");
}

void Cfg::initErrData()
{
    sErrData *errs = &(item->errs);
    errs->volErr = read("vol", 1,"Errs").toInt();
    errs->curErr = read("cur", 1,"Errs").toInt();
    errs->powErr = read("pow", 15,"Errs").toInt();
}

void Cfg::writeErrData()
{
    sErrData *errs = &(item->errs);
    write("vol", errs->volErr, "Errs");
    write("cur", errs->curErr, "Errs");
    write("pow", errs->powErr, "Errs");
}
