/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_mainwid.h"
#include "ui_home_mainwid.h"

Home_MainWid::Home_MainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_MainWid)
{
    ui->setupUi(this); //initWid();
    groupBox_background_icon(this); mSsdp = Ssdp_Core::bulid(this);
    connect(mSsdp, &Ssdp_Core::sendMsgSig, this, &Home_MainWid::onDown);
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}

void Home_MainWid::webLogin()
{

}


void Home_MainWid::initWid()
{
    QPalette pl = ui->textEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
}


void Home_MainWid::onDown(const QString &msg)
{
    QString str = QString::number(++mId) + "、"+ msg + "\n";
    ui->textEdit->moveCursor(QTextCursor::Start);
    ui->textEdit->insertPlainText(str);
}

void Home_MainWid::on_findBtn_clicked()
{
    QStringList ips = mSsdp->searchAll();
    QString str = tr("未找到任何目标设备");
    if(ips.size()) {
        str = tr("已找到%1个目标IP:\t\n").arg(ips.size());
        int i=0; foreach(const auto &ip, ips) {
            str += "   " + ip; i++;
            if(i%4==0) str += "\n";
        }
    }
    MsgBox::information(this, str);
}

void Home_MainWid::send(const sCfgItem &it, const QVariant &v)
{
    QString room, ip; //getInput(room, ip);
    if(ui->checkBox->isChecked()) ip = ui->dstEdit->text();
    mSsdp->setCfg(it, v, room, ip);
}

void Home_MainWid::on_startBtn_clicked()
{
    sCfgItem it; it.type = 14;
    QString v = ui->usrEdit->text();
    if(v.size()) {
        it.fc = 1;
        send(it, v);
    }

    v = ui->pwdEdit->text();
    if(v.size()) {
        it.fc =2;
        send(it, v);
    }
}

void Home_MainWid::devMode()
{
    sCfgItem it; it.type = 13; it.fc = 3;
    int v = ui->modeBox->currentIndex();
    send(it, v);

    if(v<3) {
        it.fc = 4;
    } else {
        it.type = 15;
        it.fc = 2;
    }

    v = ui->addrBox->value();
    send(it, v);
}

void Home_MainWid::location()
{
    sCfgItem it; it.type = 11;
    QString v = ui->roomEdit->text();
    if(v.size()) {
        it.fc = 1;
        send(it, v);
    }

    v = ui->locatEdit->text();
    if(v.size()) {
        it.fc = 2;
        send(it, v);
    }
}

void Home_MainWid::netAddr()
{
    sCfgItem it; it.type = 41; it.fc = 1;
    int id = ui->dhcpBox->currentIndex();
    send(it, id); if(id == 0) {
        QString v = ui->ipEdit->text();
        if(v.size()) {it.fc = 2; send(it, v);}

        v = ui->maskEdit->text();
        if(v.size()) {it.fc = 3; send(it, v);}

        v = ui->gwEdit->text();
        if(v.size()) {it.fc = 4; send(it, v);}

        v = ui->dnsEdit->text();
        if(v.size()) {it.fc = 6; send(it, v);}
    }
}

void Home_MainWid::integrate()
{
    ///////////=============

}

void Home_MainWid::on_dhcpBox_currentIndexChanged(int index)
{
    ui->ipEdit->setDisabled(index);
    ui->maskEdit->setDisabled(index);
    ui->gwEdit->setDisabled(index);
    ui->dnsEdit->setDisabled(index);
}


void Home_MainWid::on_pushBox_currentIndexChanged(int index)
{
    bool en = false; if(index == 1) en = true;
    ui->hostEdit->setEnabled(index);
    ui->portEdit->setEnabled(en);
}


void Home_MainWid::on_modeBox_currentIndexChanged(int index)
{
    ui->addrBox->setEnabled(index);
}


void Home_MainWid::on_checkBox_stateChanged(int arg1)
{
    ui->dstEdit->setEnabled(arg1);
}

