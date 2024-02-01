/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "cfgcom.h"

CfgCom::CfgCom(const QString &fn, QObject *parent)  : CfgSerial(fn, parent)
{
    //openCfg(parent, fn);
    item = new sCfgComIt();
    initCfgCom();
}

CfgCom *CfgCom::bulid(const QString& fn, QObject *parent)
{
    static CfgCom* sington = nullptr;
    if(!sington) {
        QString name = pathOfCfg(fn);
        sington = new CfgCom(name, parent);
    }
    return sington;
}

void CfgCom::initCfgCom()
{
    item->user = readCfg("user", "", "User").toString();
    item->pcNum = readCfg("pc_num", 0, "Sys").toInt();
}

void CfgCom::writeCfgCom()
{
    writeCfg("user", item->user, "User");
    writeCfg("pc_num", item->pcNum, "Sys");
}

void CfgCom::readMaxMinData()
{
    item->nolmin[0] = readCfg("pow_min","0","Config").toString();
    item->nolmin[1] = readCfg("vol1_min","0","Config").toString();
    item->nolmin[2] = readCfg("vol2_min","0","Config").toString();
    item->nolmin[3] = readCfg("vol3_min","0","Config").toString();
    item->nolmin[4] = readCfg("vol4_min","0","Config").toString();

    item->nolmax[0] = readCfg("pow_max","0","Config").toString();
    item->nolmax[1] = readCfg("vol1_max","0","Config").toString();
    item->nolmax[2] = readCfg("vol2_max","0","Config").toString();
    item->nolmax[3] = readCfg("vol3_max","0","Config").toString();
    item->nolmax[4] = readCfg("vol4_max","0","Config").toString();
}
void CfgCom::initMaxMinData()
{
    writeCfg("pow_min", item->nolmin[0], "Config");
    writeCfg("pow_max", item->nolmax[0], "Config");
    writeCfg("vol1_min", item->nolmin[1], "Config");
    writeCfg("vol1_max", item->nolmax[1], "Config");
    writeCfg("vol2_min", item->nolmin[2], "Config");
    writeCfg("vol2_max", item->nolmax[2], "Config");
    writeCfg("vol3_min", item->nolmin[3], "Config");
    writeCfg("vol3_max", item->nolmax[3], "Config");
    writeCfg("vol4_min", item->nolmin[4], "Config");
    writeCfg("vol4_max", item->nolmax[4], "Config");
}
