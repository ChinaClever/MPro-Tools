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
    void location();
    void netAddr();
    void integrate();
    void send(const sCfgItem &it, const QVariant &v);

private slots:
    void on_findBtn_clicked();
    void on_startBtn_clicked();
    void onDown(const QString &msg);
    void on_dhcpBox_currentIndexChanged(int index);
    void on_pushBox_currentIndexChanged(int index);

    void on_modeBox_currentIndexChanged(int index);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::Home_MainWid *ui;
    Ssdp_Core *mSsdp;
    int mId = 0;
};

#endif // HOME_MAINWID_H
