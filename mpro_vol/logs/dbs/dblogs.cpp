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
    tableTile = tr("日志"); //hiddens <<  9;
    headList << tr("PCB码") << tr("结果") << tr("数据");
}

void DbLogs::createTable()
{
    QString cmd =
            "create table if not exists %1("
            "id             INTEGER primary key autoincrement not null,"
            "date           VCHAR,"
            "time           VCHAR,"
            "sn             VCHAR,"
            "result         VCHAR,"
            "data           VCHAR);";
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
    QString cmd = "insert into %1 (date,time,sn,result,data) "
                  "values(:date,:time,:sn,:result,:data)";
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
    query.bindValue(":sn",item.sn);
    query.bindValue(":result",item.result);
    query.bindValue(":data",item.data);
    bool ret = query.exec();
    if(!ret) throwError(query.lastError());
    return ret;
}
