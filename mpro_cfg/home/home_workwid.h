#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H

#include <QWidget>
#include "logmainwid.h"
#include "cfg_app.h"
#include "ssdp_core.h"

namespace Ui {
class Home_WorkWid;
}

class Home_WorkWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_WorkWid(QWidget *parent = nullptr);
    ~Home_WorkWid();

signals:
    void startSig();
    void downSig(const QString &dir);

private:
    void initWid();
    void workDown();
    void setCfgItem();
    void setDataItem();
    bool checkInput(const QByteArray &msg);
    QByteArray readFile(const QString &fn);
    void getInput(QString &room, QString &ip);

private slots:
    void on_findBtn_clicked();
    void on_startBtn_clicked();
    void on_selectBtn_clicked();
    void onLogSlot(const QString &msg);
    void on_cfg_checkBox_clicked(bool en);
    void on_alarm_checkBox_clicked(bool en);
    void on_batchCheck_clicked(bool checked);
    void on_rangeBox_currentIndexChanged(int index);

private:
    Ui::Home_WorkWid *ui;
    Ssdp_Core *mSsdp;
};

#endif // HOME_WORKWID_H
