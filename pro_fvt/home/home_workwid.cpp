/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QStandardPaths>


Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this); initFun();
    mHttp = Core_Http::bulid(this);
    mCoreThread = new Core_Thread(this);
    QTimer::singleShot(450,this,SLOT(initFunSlot()));
    //mHttp->initHost("192.168.1.89");
}

Home_WorkWid::~Home_WorkWid()
{
    delete ui;
}

void Home_WorkWid::initLayout()
{
    QPalette pl = ui->textEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
    //ui->userEdit->setPalette(pl); //ui->cntSpin->setPalette(pl);
    //ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //ui->textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QGridLayout *gridLayout = new QGridLayout(this->parentWidget());
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(this);
}

void Home_WorkWid::initFunSlot()
{
    initLayout(); isStart = false;
    timer = new QTimer(this); //timer->start(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutDone()));
    connect(mCoreThread, &Core_Thread::finshSig, this, &Home_WorkWid::finishSlot);
    connect(mCoreThread, &Core_Thread::overSig, this, &Home_WorkWid::updateResult);
    connect(mCoreThread, &Core_Thread::msgSig, this, &Home_WorkWid::insertTextSlot);
    //QTimer::singleShot(450,this,SLOT(updateCntSlot()));
}

void Home_WorkWid::finishSlot(bool pass, const QString &msg)
{
    QString str = msg + tr(" 配置文件上传");;
    if(pass) {
        mCnt.ok += 1;
        str = tr("成功！");
    } else {
        mCnt.err += 1;
        str = tr("失败！");
    } mCnt.all += 1;
    updateCntSlot();
    insertTextSlot(str, pass);
}

void Home_WorkWid::setTextColor(bool pass)
{
    QColor color("black");
    if(!pass) {color = QColor("red"); mResult = pass;}
    ui->textEdit->moveCursor(QTextCursor::Start);

    QTextCharFormat fmt;//文本字符格式
    fmt.setForeground(color);// 前景色(即字体色)设为color色
    QTextCursor cursor = ui->textEdit->textCursor();//获取文本光标
    cursor.mergeCharFormat(fmt);//光标后的文字就用该格式显示
    ui->textEdit->mergeCurrentCharFormat(fmt);//textEdit使用当前的字符格式
}

void Home_WorkWid::insertTextSlot(const QString &msg, bool pass)
{
    QString str = QString::number(mId++) + "、"+ msg + "\n";
    setTextColor(pass); ui->textEdit->insertPlainText(str);
}

void Home_WorkWid::updateCntSlot()
{
    sCount *cnt = &mCnt;
    ui->okLcd->display(cnt->ok);
    ui->allLcd->display(cnt->all);
    ui->errLcd->display(cnt->err);
    ui->cntSpin->setValue(cnt->cnt);

    QString str = "0";
    if(cnt->all) {
        double value = cnt->ok*100.0 / cnt->all;
        str = QString::number(value,'f',0) +"%";
    } ui->passLcd->display(str);
}


QString Home_WorkWid::getTime()
{
    QTime t(0,0,0,0);
    t = t.addSecs(startTime.secsTo(QTime::currentTime()));
    return  tr("%1").arg(t.toString("mm:ss"));
}

void Home_WorkWid::updateTime()
{
    QString str = getTime();
    QString style = "background-color:yellow; color:rgb(0, 0, 0);";
    style += "font:100 34pt \"微软雅黑\";";

    ui->timeLab->setText(str);
    ui->timeLab->setStyleSheet(style);
}

void Home_WorkWid::setWidEnabled(bool en)
{
    ui->groupBox->setEnabled(en);
}


void Home_WorkWid::updateResult()
{
    QString style; mId = 1;
    QString str = tr("---");
    if(mResult) {
        str = tr("成功"); style = "background-color:green; color:rgb(255, 255, 255);";
    } else {
        str = tr("失败"); style = "background-color:red; color:rgb(255, 255, 255);";
    } style += "font:100 34pt \"微软雅黑\";";

    isStart = false;
    setWidEnabled(true);
    ui->timeLab->setText(str);
    ui->timeLab->setStyleSheet(style);
    ui->startBtn->setText(tr("开 始"));
    QTimer::singleShot(450,this,SLOT(updateCntSlot()));
    str = QTime::currentTime().toString("hh:mm:ss");
    ui->endLab->setText(str);
    timer->stop();
}

bool Home_WorkWid::initMac()
{
    bool ret = true;
    sMac *it = MacAddr::bulid()->macItem;
    int res =  MacAddr::bulid()->macCnt(it->mac, it->endMac);
    if(res <= it->cntMac) {
        if(res < 1) {
            MsgBox::critical(this, tr("MAC地址已用完，无法继续使用")); ret = false;
        } else {
            QString str = tr("剩余MAC地址，仅有%1个，请向领导反馈").arg(res);
            MsgBox::critical(this, str);
        }
    }

    return ret;
}


bool Home_WorkWid::initUser()
{
    QString str = ui->userEdit->text();
    if(str.isEmpty()){MsgBox::critical(this, tr("请先填写客户名称！")); return false;}

    int cnt = ui->cntSpin->value();
    if(cnt < 1) {MsgBox::critical(this, tr("请先填写订单剩余数量！")); return false;}
    return true;
}


bool Home_WorkWid::inputCheck()
{
    bool ret = false;
    QString str = ui->ipEdit->text();
    if(ui->adCheckBox->isChecked()) {

    } else {
        ret = cm_isIPaddress(str);
        if(!ret) MsgBox::critical(this, tr("目标设备IP出错！"));
    }

    return ret;
}


void Home_WorkWid::initData()
{
    mId = 1; mResult = true;
}


bool Home_WorkWid::initWid()
{
    //bool ret = initMac(); ////======
    bool ret = true;
    if(ret) ret = initUser();
    if(ret) ret = inputCheck();
    if(ret) {
        initData();
        updateWid();
        setWidEnabled(false);
        ui->startBtn->setText(tr("终 止"));
        startTime = QTime::currentTime(); emit startSig();
        QString str = startTime.toString("hh:mm:ss");
        ui->startLab->setText(str);
        ui->endLab->setText("---");
        ui->textEdit->clear();
        isStart = true;
    }

    return ret;
}



bool Home_WorkWid::updateWid()
{
    QString dir = "usr/data/clever";
    Cfg_App cfg(dir, this); sAppVerIt it;
    bool ret = cfg.app_unpack(it);



/*
    QString str = mDt->serialNumber;
    if(str.isEmpty()) str = "--- ---";
    ui->snLab->setText(str);

    str = mDt->manufacturer;
    if(str.isEmpty()) str = "--- ---";
    ui->mfLab->setText(str);

    str = mDt->model;
    if(str.isEmpty()) str = "--- ---";
    ui->modelLab->setText(str);

    str = mDt->hwRevision;
    if(str.isEmpty()) str = "--- ---";
    ui->hwLab->setText(str);

    str = mDt->fwRevision;
    if(str.isEmpty()) str = "--- ---";
    ui->fwLab->setText(str);

    str = mDt->ctrlBoardSerial;
    if(str.isEmpty()) str = "--- ---";
    ui->ctrlSnLab->setText(str);

    str = mDt->macAddress;
    if(str.isEmpty()) str = "--- ---";
    ui->macLab->setText(str);

    if(mPro->step < Test_Over) {
        updateTime();
    } else if(mPro->step < Test_End){
        updateResult();
    }
    */
}



void Home_WorkWid::timeoutDone()
{
    updateTime();
}


void Home_WorkWid::on_startBtn_clicked()
{
    if(isStart == false) {
        if(initWid()) {
            timer->start(500);
            mCoreThread->start();
            //mHttp->uploadFile(m_fs);
        }
    } else {
        bool ret = MsgBox::question(this, tr("确定需要提前结束？"));
        if(ret) { mResult = false; updateResult(); }
    }
}

void Home_WorkWid::writeMac(const QByteArray &mac)
{
    QFile file(mDir+"mac.ini");
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(mac); file.close();
    }
}


void Home_WorkWid::on_findBtn_clicked()
{
    QString room, ip; //getInput(room, ip);
    Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
    QStringList ips = ssdp->searchTarget(room, ip);
    QString str = tr("未找到任何目标设备");
    if(ips.size()) {
        str = tr("已找到%1个设备，包含%2个主机，%3个副机\t\n")
                .arg(ips.size() + ssdp->getSlaveNum())
                .arg(ips.size()).arg(ssdp->getSlaveNum());
        int i=0; foreach(const auto &ip, ips) {
            str += "   " + ip; i++;
            if(i%4==0) str += "\n";
        }
    }
    MsgBox::information(this, str);
}


auto Home_WorkWid::initFun()
{
    QString dir = "usr/data/clever/cfg/";
    FileMgr::build().mkpath(dir); QStringList fs;
    fs << dir+"alarm.cfg" << dir+"devParam.ini" << dir+"cfg.ini" << dir+"logo.png";
    fs << dir+"inet.ini" << dir+"alarm.df" << dir+"snmpd.conf";// << mDir+"mac.ini";
    fs << "usr/data/clever/ver.ini";
    return fs;
}


void Home_WorkWid::on_downBtn_clicked()
{
    QString str = tr("请确认下载设备的配置文件?");
    if(MsgBox::question(this, str)) {
        QStringList fs = initFun();  emit startSig();
        FileMgr::build().delFileOrDir("usr/data/clever");
        cm_mdelay(10); mHttp->downFile(fs);
    }
}

