#ifndef HOME_MAINWID_H
#define HOME_MAINWID_H

#include <QWidget>
#include "jsonrpcclient.h"

namespace Ui {
class Home_MainWid;
}

class Home_MainWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_MainWid(QWidget *parent = nullptr);
    ~Home_MainWid();

private:
    void initWid();
    bool inputCheck();
    void enableWid(bool en);

private slots:
    void on_swBtn_clicked();
    void on_cfgBtn_clicked();
    void on_cmdBtn_clicked();
    void on_logBtn_clicked();
    void on_alrmBtn_clicked();
    void on_connectBtn_clicked();
    void onDown(const QVariant &msg);
    void on_cfgBox_currentIndexChanged(int index);
    void on_alarmBox_currentIndexChanged(int index);

private:
    Ui::Home_MainWid *ui;
    JsonRpcClient *mRpc;
};

#endif // HOME_MAINWID_H
