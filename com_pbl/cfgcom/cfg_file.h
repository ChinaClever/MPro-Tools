#ifndef CFG_FILE_H
#define CFG_FILE_H
#include "cfg_obj.h"

namespace File {
    QString md5(const QString &fn);
    QStringList entryList(const QString &fn);
    uint fileSize(const QString &fn);
    QString certFile();
    QString keyFile();
};

#endif // CFG_FILE_H
