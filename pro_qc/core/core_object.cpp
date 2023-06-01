/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_object.h"

sCoreItem Core_Object::coreItem;
Core_Object::Core_Object(QObject *parent)
    : QThread{parent}
{
    mHttp = Core_Http::bulid(this);
}

void Core_Object::clearAllEle()
{
    sDataItem it;
    it.id = 0;
    it.type = 3;
    it.topic = 5;
    it.subtopic = 1;
    mHttp->setting(it);
}

void Core_Object::factoryRestore()
{
    sCfgItem it;
    it.type = 96; it.fc = 2;
    mHttp->setting(it, 1);
}

void Core_Object::relayCtrl(int on, int id)
{
    sDataItem it;
    it.id = id;
    it.type = 3;
    it.topic = 1;
    it.subtopic = 1;
    it.value = on;
    mHttp->setting(it);
}
