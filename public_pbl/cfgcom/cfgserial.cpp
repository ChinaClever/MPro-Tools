/*
 * 配置文件公共基类
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "cfgserial.h"

CfgSerial::CfgSerial(QObject *parent) : CfgCom(parent)
{

}

/**
 * @brief 获取串口名称
 * @return 串口名
 */
QString CfgSerial::getSerialName(const QString &key)
{
    return read(key, "", "Serial").toString();
}

/**
 * @brief 设置串口名
 * @param name
 */
void CfgSerial::setSerialName(const QString &key, const QString &v)
{
    write(key, v, "Serial");
}

QString CfgSerial::getSerialBr(const QString &com)
{
    QString key = QString("BR_%1").arg(com);
    return read(key, "", "Serial").toString();
}

void CfgSerial::setSerialBr(const QString &com, const QString &br)
{
    QString key = QString("BR_%1").arg(com);
    write(key, br, "Serial");
}

/**
 * @brief 获取当前用户名称
 * @return 用户名
 */
QString CfgSerial::getLoginName()
{
    return read("name", "admin", "Login").toString();
}

/**
 * @brief 设置当前用户名
 * @param name
 */
void CfgSerial::setLoginName(const QString &name)
{
    write("name", name, "Login");
}
