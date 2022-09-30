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

private slots:
    void onDown(const QString &msg);

    void on_findBtn_clicked();

private:
    Ui::Home_MainWid *ui;
    Ssdp_Core *mSsdp;
    int mId = 0;
};

#endif // HOME_MAINWID_H
