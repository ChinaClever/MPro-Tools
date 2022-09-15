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
    tableTile = tr("设置日志");
    //hiddens <<  9;
    headList << tr("目标设备") << tr("设置内容");
}

void DbLogs::createTable()
{
    QString cmd =
            "create table if not exists %1("
            "id             INTEGER primary key autoincrement not null,"
            "date           VCHAR,"
            "time           VCHAR,"
            "dst            VCHAR,"
            "content        VCHAR);";
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
    QString cmd = "insert into %1 (date,time,dst,content) "
                  "values(:date,:time,:dst,:content)";
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
    query.bindValue(":dst",item.dst);
    query.bindValue(":content",item.content);
    bool ret = query.exec();
    if(!ret) throwError(query.lastError());
    return ret;
}
