#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H

#include <QWidget>
#include "logmainwid.h"
#include "core_httpobj.h"

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
    void initFun();
    void writeMac(const QByteArray &mac);

private slots:
    void on_startBtn_clicked();
    void on_findBtn_clicked();
    void on_downBtn_clicked();

private:
    Ui::Home_WorkWid *ui;
    Core_HttpObj *mHttp;
    QStringList m_fs;
    QString mDir;
};

#endif // HOME_WORKWID_H
