/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QStandardPaths>
#include <QFileDialog>

Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this); initWid();
    mSsdp = Ssdp_Core::bulid(this);
    connect(mSsdp, &Ssdp_Core::sendDataSig, this, &Home_WorkWid::onLogSlot);
}

Home_WorkWid::~Home_WorkWid()
{
    delete ui;
}

void Home_WorkWid::initWid()
{
    bool en = ui->alarm_checkBox->isChecked();
    on_alarm_checkBox_clicked(en);

    en = ui->cfg_checkBox->isChecked();
    on_cfg_checkBox_clicked(en);
    on_batchCheck_clicked(en);
}


void Home_WorkWid::setDataItem()
{
    if(ui->alarm_checkBox->isChecked()) {
        sDataItem it;
        it.id = ui->alarm_idBox->value();
        it.addr = ui->alarm_addrBox->value();
        it.type = ui->alarm_typeBox->value();
        it.topic = ui->alarm_topicBox->value();
        it.subtopic = ui->alarm_subBox->value();
        it.value = ui->alarm_valueSpinBox->value();

        QString room, ip; getInput(room, ip);
        mSsdp->setting(it, room, ip);
    }
}

void Home_WorkWid::setCfgItem()
{
    if(ui->cfg_checkBox->isChecked()) {
        sCfgItem it;
        it.addr = ui->cfg_addrBox->value();
        it.type = ui->cfg_typeBox->value();
        it.id = ui->cfg_idBox->value();
        it.fc = ui->cfg_fcBox->value();
        QString v = ui->cfg_valueEdit->text();
        QString room, ip; getInput(room, ip);
        mSsdp->setCfg(it, v, room, ip);
    }
}

QByteArray Home_WorkWid::readFile(const QString &fn)
{
    QFile file(fn); QByteArray res;
    bool ret = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(ret) res = file.readAll();
    else cout << file.errorString();
    file.close();
    return res;
}

bool Home_WorkWid::checkInput(const QByteArray &msg)
{
    QJsonObject obj;
    QJsonParseError jsonerror; bool ret = false;
    QJsonDocument doc = QJsonDocument::fromJson(msg, &jsonerror);
    if (!doc.isNull() && jsonerror.error == QJsonParseError::NoError) {
        if(doc.isObject()) {
            obj = doc.object();
            ret = true;
        }
    }
    return ret;
}


void Home_WorkWid::workDown()
{
    QString dir = ui->pathEdit->text();
    QString room, ip; getInput(room, ip);
    if(dir.size() && ui->batchCheck->isChecked()) {
        QStringList ls = File::entryList(dir);
        foreach(const auto &f, ls) {
            QString fn = dir + "/" + f;
            QByteArray res = readFile(fn);
            if(checkInput(res)) mSsdp->setJson(res, room, ip);
        }
    }
}

void Home_WorkWid::on_startBtn_clicked()
{
    emit startSig();
    setDataItem();
    setCfgItem();
    workDown();
}

void Home_WorkWid::on_selectBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->pathEdit->setText(dir);
}

void Home_WorkWid::on_rangeBox_currentIndexChanged(int index)
{
    bool en = false;
    if(index) en = true;
    ui->dstEdit->setEnabled(en);
}

void Home_WorkWid::onLogSlot(const QString &msg)
{
    sLogItem it;
    it.content = msg;

    QString dst = "all";
    QString room, ip; getInput(room, ip);
    if(ip.size()) dst = "IP:" + ip;
    else if(room.size()) dst = "room:" + room;
    it.dst = dst;

    DbLogs::bulid()->insertItem(it);
}

void Home_WorkWid::getInput(QString &room, QString &ip)
{
    int index = ui->rangeBox->currentIndex();
    if(index == 1) ip = ui->dstEdit->text();
    else if(index == 2) room = ui->dstEdit->text();
}

void Home_WorkWid::on_findBtn_clicked()
{
    QString room, ip; getInput(room, ip);
    QStringList ips = mSsdp->searchTarget(room, ip);
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

void Home_WorkWid::on_cfg_checkBox_clicked(bool en)
{
    ui->cfg_valueEdit->setEnabled(en);
    ui->cfg_addrBox->setEnabled(en);
    ui->cfg_typeBox->setEnabled(en);
    ui->cfg_idBox->setEnabled(en);
    ui->cfg_fcBox->setEnabled(en);
}


void Home_WorkWid::on_alarm_checkBox_clicked(bool en)
{
    ui->alarm_idBox->setEnabled(en);
    ui->alarm_subBox->setEnabled(en);
    ui->alarm_addrBox->setEnabled(en);
    ui->alarm_typeBox->setEnabled(en);
    ui->alarm_topicBox->setEnabled(en);
    ui->alarm_valueSpinBox->setEnabled(en);
}

void Home_WorkWid::on_batchCheck_clicked(bool checked)
{
    ui->pathEdit->setEnabled(checked);
    ui->selectBtn->setEnabled(checked);
}

