#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H

#include <QWidget>
#include "logmainwid.h"
#include "core_thread.h"

namespace Ui {
class Home_WorkWid;
}

struct sCount
{
    sCount() {all=ok=err=cnt=0;}

    int cnt;
    int all;
    int ok;
    int err;
};

class Home_WorkWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_WorkWid(QWidget *parent = nullptr);
    ~Home_WorkWid();

signals:
    void startSig();

private:
    auto initFun();
    bool initMac();
    bool initWid();
    bool initUser();
    bool inputCheck();
    void initData();

    QString getTime();
    void initLayout();
    void updateTime();
    bool updateWid();
    void setWidEnabled(bool en);
    void setTextColor(bool pass);

private slots:
    void timeoutDone();
    void initFunSlot();
    void updateCntSlot();
    void updateResult();
    void on_startBtn_clicked();
    void on_findBtn_clicked();
    void on_downBtn_clicked();
    void finishSlot(bool pass, const QString &msg);
    void insertTextSlot(const QString &msg, bool pass);
    void on_adCheckBox_clicked(bool checked);

private:
    Ui::Home_WorkWid *ui;
    Core_Thread *mCoreThread;
    QTimer *timer;
    sCount mCnt;
    bool isStart;
    bool mResult;
    QTime startTime;
    int mId=1;
};

#endif // HOME_WORKWID_H
