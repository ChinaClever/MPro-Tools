#include "home_mainwid.h"
#include "ui_home_mainwid.h"
#include "backcolourcom.h"
#include "print.h"

Home_MainWid::Home_MainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_MainWid)
{
    ui->setupUi(this);
    mRpc = new JsonRpcClient(this);
    groupBox_background_icon(this);
    enableWid(false); initWid();
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}

void Home_MainWid::initWid()
{
    QPalette pl = ui->textEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
    connect(mRpc, &JsonRpcClient::rpcSig, this, &Home_MainWid::onDown);
}

void Home_MainWid::on_alrmBtn_clicked()
{
    uchar addr = ui->addrSpinBox->value();
    uchar type = ui->alarmTypeBox->value();
    uchar topic = ui->alrmTopicBox->value();
    uchar sub = ui->alarmSubBox->value();
    uchar id = ui->alarmIdBox->value();
    uint value = ui->alarmValueBox->value();

    int index =  ui->alarmBox->currentIndex();
    if(index) mRpc->pduDataSet(addr, type, topic, sub, id, value);
    else mRpc->pduDataGet(addr, type, topic, sub, id);
}


void Home_MainWid::on_cfgBtn_clicked()
{
    uchar addr = ui->addrSpinBox->value();
    uchar type = ui->cfgTypeBox->value();
    uchar fc = ui->cfgFcBox->value();
    uchar id = ui->cfgIdBox->value();
    QVariant value = ui->cfgEdit->text();

    int index =  ui->cfgBox->currentIndex();
    if(index) mRpc->pduCfgSet(type, fc, value, addr, id);
    else mRpc->pduCfgGet(type, fc, addr, id);
}


void Home_MainWid::on_cmdBtn_clicked()
{
    QString cmd = ui->cmdEdit->text();
    mRpc->execute(cmd);
}


void Home_MainWid::on_connectBtn_clicked()
{
    static bool isConnected = false;
    QString str = tr("连接");
    if(isConnected) {
        isConnected = false;
        mRpc->close();
    } else {
        isConnected = inputCheck();
        if(isConnected) str = tr("断开");
    }  ui->connectBtn->setText(str);
    enableWid(isConnected);
}

bool Home_MainWid::inputCheck()
{
    QString ip = ui->ipEdit->text();
    bool ret = cm_isIPaddress(ip);
    if(ret){
        ret = mRpc->startClient(ip, ui->portBox->value());
        if(!ret) MsgBox::critical(this, tr("无法连接服务端！！！"));
    } else  MsgBox::critical(this, tr("请正确输入IP地址！！！"));
    return ret;
}

void Home_MainWid::enableWid(bool en)
{
    ui->readBtn->setEnabled(en);
    ui->portBox->setDisabled(en);
    ui->ipEdit->setDisabled(en);
    ui->groupBox_1->setEnabled(en);
    ui->groupBox_2->setEnabled(en);
    ui->groupBox_3->setEnabled(en);
    ui->groupBox_4->setEnabled(en);
    ui->groupBox_5->setEnabled(en);
}

void Home_MainWid::on_alarmBox_currentIndexChanged(int index)
{
    ui->alarmValueBox->setEnabled(index);
}

void Home_MainWid::on_cfgBox_currentIndexChanged(int index)
{
    ui->cfgEdit->setEnabled(index);
}

void Home_MainWid::onDown(const QVariant &msg)
{
    ui->textEdit->clear();
    ui->textEdit->insertPlainText(msg.toString());
}

