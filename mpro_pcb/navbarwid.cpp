#include "navbarwid.h"
#include "ui_navbarwid.h"

Navbarwid::Navbarwid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Navbarwid)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/title_back.jpg");
    QGridLayout *gridLayout = new QGridLayout(parent);
    gridLayout->setContentsMargins(0, 0, 0, 6);
    gridLayout->addWidget(this);
    mUserLand = new UsrLandDlg(this);
    //ui->devBtn->setHidden(true);
    QTimer::singleShot(5,this,SLOT(on_loginBtn_clicked()));
    connect(mUserLand,SIGNAL(sendUserNameSig(QString)),this,SLOT(recvUserNameSlot(QString)));
}

Navbarwid::~Navbarwid()
{
    delete ui;
}

void Navbarwid::on_homeBtn_clicked()
{
    emit navBarSig(0);
}


void Navbarwid::on_setBtn_clicked()
{
    emit navBarSig(1);
}

void Navbarwid::on_logBtn_clicked()
{
    emit navBarSig(2);
}

void Navbarwid::on_loginBtn_clicked()
{
    bool lang = LandingUser::get()->land;
    if(lang) {
        int ret = mUserLand->selectWork();
        if(ret == 1) { // 用户切换
            mUserLand->exec();
        }  else if(ret == 2) { // 用户退出
            mUserLand->quitWidget();
        }
    } else {
        mUserLand->exec();
    }
}


void Navbarwid::recvUserNameSlot(QString str)
{
    ui->userLab->setText(str);
}

