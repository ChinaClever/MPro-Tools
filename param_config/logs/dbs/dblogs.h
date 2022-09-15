#ifndef DBLOGS_H
#define DBLOGS_H
#include "basicsql.h"

struct sLogItem : public DbBasicItem{
    QString dst, content;
};

class DbLogs : public SqlBasic<sLogItem>
{
    DbLogs();
public:
    static DbLogs* bulid();
    QString tableName(){return "sets";}
    bool insertItem(const sLogItem& item);

protected:
    void createTable();
    bool modifyItem(const sLogItem& item,const QString& cmd);
};

#endif // DBLOGS_H
