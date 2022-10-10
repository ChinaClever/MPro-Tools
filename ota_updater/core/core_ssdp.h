#ifndef CORE_SSDP_H
#define CORE_SSDP_H

#include "ssdp_search.h"

class Core_Ssdp : public Ssdp_Search
{
    Q_OBJECT
    explicit Core_Ssdp(QObject *parent = nullptr);
public:
    static Core_Ssdp *bulid(QObject *parent = nullptr);
};

#endif // CORE_SSDP_H
