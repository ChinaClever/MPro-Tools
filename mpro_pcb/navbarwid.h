#ifndef NAVBARWID_H
#define NAVBARWID_H

#include <QtCore>
#include <QWidget>
#include "usrlanddlg.h"

namespace Ui {
class Navbarwid;
}

class Navbarwid : public QWidget
{
    Q_OBJECT

public:
    explicit Navbarwid(QWidget *parent = nullptr);
    ~Navbarwid();
signals:
    void navBarSig(int);
private slots:
    void on_homeBtn_clicked();
    void on_setBtn_clicked();
    void on_logBtn_clicked();
    void on_loginBtn_clicked();
    void recvUserNameSlot(QString str);

private:
    Ui::Navbarwid *ui;
    UsrLandDlg* mUserLand;
};

#endif // NAVBARWID_H
