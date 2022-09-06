/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "core_ssdp.h"

Core_Ssdp::Core_Ssdp(QObject *parent)
    : Ssdp_Server{parent}
{
    bind();
}


Core_Ssdp *Core_Ssdp::bulid(QObject *parent)
{
    static Core_Ssdp* sington = nullptr;
    if(!sington) sington = new Core_Ssdp(parent);
    return sington;
}
