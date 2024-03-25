#ifndef HOME_MAINWID_H
#define HOME_MAINWID_H

#include <QWidget>
#include "cfg_app.h"
#include "ssdp_core.h"
#include "backcolourcom.h"

namespace Ui {
class Home_MainWid;
}

class Home_MainWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_MainWid(QWidget *parent = nullptr);
    ~Home_MainWid();
protected:    
    void initWid();
    void devMode();
    void webLogin();
    void netAddr();
    void setWorkDown();
    void setDateTime();
    void setEnableWid(bool en);
    bool checkFile(const QByteArray &msg);
    QByteArray readFile(const QString &fn);
    void send(const sCfgItem &it, const QVariant &v);
    void incrementIPAddress();
    void incrementAddr();
    void disable(bool v);
    void v2_able();
    void v3_able();
    void modbusTcp();
    void snmp();
    void modbusRtu();
    void reboot();
private slots:
    void on_findBtn_clicked();
    void on_startBtn_clicked();
    void onDown(const QString &msg);
    void on_dhcpBox_currentIndexChanged(int index);
    // svoid on_pushBox_currentIndexChanged(int index);
    void on_modeBox_currentIndexChanged(int index);
    void on_batchCheck_clicked(bool checked);
    void on_checkBox_stateChanged(int arg1);
    void on_selectBtn_clicked();

    // void on_rangeBox_currentIndexChanged(int index);

    void on_snmpBox_currentIndexChanged(int index);
    void on_rtuBox_currentIndexChanged(int index);
    void on_tcpBox_currentIndexChanged(int index);
    void on_addrBox_currentIndexChanged(int index);

    void on_webCheckBox_stateChanged(int arg1);
    void on_netcheckBox_stateChanged(int arg1);
    void on_tcpcheckBox_stateChanged(int arg1);
    void on_snmpcheckBox_stateChanged(int arg1);
    void on_rtucheckBox_stateChanged(int arg1);

private:
    Ui::Home_MainWid *ui;
    Ssdp_Core *mSsdp;
    QTimer *timer=nullptr;
    int mId = 0;
};

#endif // HOME_MAINWID_H
