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
    ui->setupUi(this);
    QTimer::singleShot(450,this,SLOT(initFunSlot()));
    mCorethread = Core_Thread::bulid(this);
    // mItem = new sCfgComIt();
    mItem = CfgCom::bulid()->item;
    //Core_Http::bulid(this)->initHost("192.168.1.89");
    // Core_Http::bulid(this)->execute("killall cores");
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

    QGridLayout *gridLayout = new QGridLayout(this->parentWidget());
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(this);
}

void Home_WorkWid::initFunSlot()
{
    initLayout(); isStart = false;
    timer = new QTimer(this); //timer->start(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutDone()));
    // QTimer::singleShot(1715,this,SLOT(updateWidSlot()));
    connect(mCorethread, &Core_Thread::finshSig, this, &Home_WorkWid::finishSlot);
    connect(mCorethread, &Core_Thread::overSig, this, &Home_WorkWid::updateResult);
    connect(mCorethread, &Core_Thread::msgSig, this, &Home_WorkWid::insertTextSlot);
}

void Home_WorkWid::logWrite()
{
    sLogItem logIt;
    QString str = tr("T1 = %1°C，T2 = %2°C，T3 = %3°C，T4 = %4°C，门禁1 =%5，门禁2 =%6，执行板版本号：%7，%8").arg(mItem->tem[0])
                      .arg(mItem->tem[1]).arg(mItem->tem[2]).arg(mItem->tem[3]).arg(mItem->doors[0])
                      .arg(mItem->doors[1]).arg(mItem->op_ver).arg(mItem->serial);

    logIt.data = str;
    if(mResult) logIt.result = tr("通过");
    else logIt.result = tr("失败");
    DbLogs::bulid()->insertItem(logIt);
}

void Home_WorkWid::finishSlot(bool pass, const QString &msg)
{
    QString str = msg;
    mResult = pass;
    if(pass) {
        mItem->ok += 1;
        str += tr("成功！");
    } else {
        mItem->err += 1;
        str += tr("失败！");
    } mItem->all += 1;
    insertTextSlot(str, pass);
    updateCntSlot();
    logWrite();
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

    // mPro->init();
    QString str = QString::number(mId++) + "、"+ msg + "\n";
    setTextColor(pass); ui->textEdit->insertPlainText(str);

}
void Home_WorkWid::updateCntSlot()
{
    sCfgComIt *cnt = mItem;
    ui->okLcd->display(cnt->ok);
    ui->allLcd->display(cnt->all);
    ui->errLcd->display(cnt->err);

    QString str = "0"; if(cnt->all) {
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
    ui->timeLab->setText(str);
    ui->timeLab->setStyleSheet(style);
    ui->startBtn->setText(tr("开 始"));
    //QTimer::singleShot(450,this,SLOT(updateCntSlot()));
    str = QTime::currentTime().toString("hh:mm:ss");
    ui->endLab->setText(str);
    timer->stop();
}


bool Home_WorkWid::initWid()
{
    bool ret = true ;
    if(ret) {
        mResult = true;
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

void Home_WorkWid::updateWidSlot()
{
}

void Home_WorkWid::timeoutDone()
{
    updateTime();
}

void Home_WorkWid::on_startBtn_clicked()
{
    bool ret = true;
    if(isStart == false) {
        if(initWid()){
            timer->start(500);
            mCorethread->start();
        }
    }else{
        ret = MsgBox::question(this, tr("确定需要提前结束？"));
        if(ret) {
            mItem->result = 0;
            mResult = false;
            updateResult();
        }
    }
}
void Home_WorkWid::saveFunSlot()
{
}


