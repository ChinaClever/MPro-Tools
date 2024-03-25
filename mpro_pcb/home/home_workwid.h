#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H

#include <QWidget>
#include "usermainwid.h"
#include "logmainwid.h"
#include "core_thread.h"
#include "backcolourcom.h"

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

private:
    bool initWid();
    QString getTime();
    void initLayout();
    void updateTime();
    bool updateWid();
    void setWidEnabled(bool en);
    void setTextColor(bool pass);
    void logWrite();
private slots:
    void updateWidSlot();
    void timeoutDone();
    void initFunSlot();
    void updateCntSlot();
    void updateResult();
    void finishSlot(bool pass, const QString &msg);
    void insertTextSlot(const QString &msg, bool pass);

    void saveFunSlot();
    void on_startBtn_clicked();

private:
    Ui::Home_WorkWid *ui;
    QTimer *timer=nullptr;
    bool isStart=false;
    bool mResult=true;
    QTime startTime;
    int mId=1;
    sCfgComIt *mItem;
    Core_Thread *mCorethread;
};

#endif // HOME_WORKWID_H
