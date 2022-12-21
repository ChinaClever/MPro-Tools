#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H

#include <QWidget>
#include "logmainwid.h"
#include "cfg_app.h"

namespace Ui {
class Home_WorkWid;
}

class Home_WorkWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_WorkWid(QWidget *parent = nullptr);
    ~Home_WorkWid();

private:
    void createDir();

private slots:
    void on_startBtn_clicked();

private:
    Ui::Home_WorkWid *ui;
};

#endif // HOME_WORKWID_H
