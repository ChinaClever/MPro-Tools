#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H
#include "cfg_file.h"
#include "home_statewid.h"
#include "core_sender.h"
#include <QHash>

namespace Ui {
class Home_WorkWid;
}

struct sCount
{
    int allCnt;
    int dstCnt;
    int slaveCnt;
    int okCnt;
    int errCnt;
};

class Home_WorkWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_WorkWid(QWidget *parent = nullptr);
    ~Home_WorkWid();

signals:
    void startSig();

protected:
    void initLayout();
    QString getTime();
    void updateTime();
    void setWidEnabled(bool en);
    void setTextColor(bool pass);

    bool initWid();
    bool inputCheck();
    void initData(sOtaFile &it);
    bool fileCrc(const QString &fn);
    void initCnt(int dst, int slave);
    bool readRsaSig(const QString &name);
    QStringList getIpList();
    bool excelMode() const;
    bool loadExcelIps(const QString &fn);
    void rememberExcelFail(const QString &msg);
    void markExcelIp(const QString &ip, const QString &result);
    void writeExcelResult();

private slots:
    void timeoutDone();
    void initFunSlot();    
    void updateResult();

    void updateCntSlot();
    void on_imgBtn_clicked();
    void on_startBtn_clicked();
    void on_searchBtn_clicked();
    void updateWidSlot(){initData(mFileIt);}
    void finishSlot(bool pass, const QString &msg);
    void on_searchBox_currentIndexChanged(int index);
    void insertTextSlot(bool pass, const QString &msg);
    void upgradeError(const QString &host, const QString &msg);

    void on_excelBtn_clicked();

private:
    struct sExcelIpRow {
        int row = 0;
        QString ip;
        QString start;
        QString end;
        QString result;
    };

    Ui::Home_WorkWid *ui;

    int mId;
    sCount mCnt;
    bool isStart;
    bool mResult;
    QTimer *timer;
    QTime startTime;
    Ssdp_Core *mSsdp;
    sOtaFile mFileIt;
    Core_Sender *mSender;
    QString mExcelFile;
    QString mExcelSheet;
    QStringList mExcelIps;
    QList<sExcelIpRow> mExcelRows;
    QHash<QString, QString> mExcelFail;
    int mExcelHeaderRow = 0;
    int mExcelIpCol = 0;
    int mColStart = 0;
    int mColEnd = 0;
    int mColResult = 0;

};

#endif // HOME_WORKWID_H
