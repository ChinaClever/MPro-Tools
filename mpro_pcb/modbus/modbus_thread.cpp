#include "modbus_thread.h"
#include <QByteArray>
Modbus_Thread::Modbus_Thread(QObject *parent) : QThread(parent)
{
    isRun = false;

    mItem = CfgCom::bulid()->item;
    mModbus = Ad_Modbus::bulid(this);
}

Modbus_Thread::~Modbus_Thread()
{
    if(isRun) {
        isRun = false;
        mItem->step = Test_Over;
    }
    wait();
}

Modbus_Thread *Modbus_Thread::bulid(QObject *parent)
{
    static Modbus_Thread* sington = nullptr;
    if(sington == nullptr)
        sington = new Modbus_Thread(parent);
    return sington;
}


void Modbus_Thread::workDown()
{

}
void Modbus_Thread::run()
{
    if(!isRun) {
        isRun = true;
        workDown();
    }
    isRun = false; mItem->step = Test_End;
    emit overSig();
}
