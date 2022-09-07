/*
 *
 *
 *  Created on: 2020年10月11日
 *      Author: Lzy
 */
#include "dblogs.h"

DbLogs::DbLogs()
{
    createTable();
    tableTile = tr("状态日志");
    //hiddens <<  9;
    headList << tr("设备类型") << tr("设备序列号") << tr("客户名称") << tr("软件版本") << tr("结果");
}

void DbLogs::createTable()
{
    QString cmd =
            "create table if not exists %1("
            "id             INTEGER primary key autoincrement not null,"
            "date           VCHAR,"
            "time           VCHAR,"
            "pn             VCHAR,"
            "sn             VCHAR not null,"
            "user           VCHAR,"
            "fw             VCHAR,"
            "result         VCHAR);";
    QSqlQuery query(mDb);
    if(!query.exec(cmd.arg(tableName()))) {
        throwError(query.lastError());
    }
}


DbLogs *DbLogs::bulid()
{
    static DbLogs* sington = nullptr;
    if(sington == nullptr)
        sington = new DbLogs();
    return sington;
}

bool DbLogs::insertItem(const sLogItem &item)
{
    QString cmd = "insert into %1 (date,time,pn,sn,user,fw,result) "
                  "values(:date,:time,:pn,:sn,:user,:fw,:result)";
    bool ret = modifyItem(item,cmd.arg(tableName()));
    if(ret) emit itemChanged(item.id, Insert);
    return ret;
}

bool DbLogs::modifyItem(const sLogItem &item, const QString &cmd)
{
    QSqlQuery query(mDb);
    query.prepare(cmd);

    query.bindValue(":date",item.date);
    query.bindValue(":time",item.time);
    query.bindValue(":pn",item.pn);
    query.bindValue(":sn",item.sn);
    query.bindValue(":user",item.user);
    query.bindValue(":fw",item.fw);
    query.bindValue(":result",item.result);
    bool ret = query.exec();
    if(!ret) throwError(query.lastError());
    return ret;
}
