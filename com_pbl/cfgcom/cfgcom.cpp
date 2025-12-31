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

    item->labelPrint = readCfg("label_print", 0, "Label").toInt(); // 0为未选中，1为选中
    item->meta = readCfg("upload_meta", "", "Upload").toString();
    item->ipAddr = readCfg("upload_ip", "127.0.0.1", "Upload").toString();
}

void CfgCom::writeCfgCom()
{
    writeCfg("user", item->user, "User");
    writeCfg("pc_num", item->pcNum, "Sys");

    writeCfg("label_print", item->labelPrint, "Label");
    writeCfg("upload_meta", item->meta, "Upload");
    writeCfg("upload_ip", item->ipAddr, "Upload");
}
