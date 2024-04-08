/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QStandardPaths>

bool mflag=true;

Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this);
    modthread = Modbus_Thread::bulid();
    QTimer::singleShot(450,this,SLOT(initFunSlot()));
    //Core_Http::bulid(this)->initHost("192.168.1.89");
    //Core_Http::bulid(this)->execute("killall cores");
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

void Home_WorkWid::

    initFunSlot()
{
    mItem = CfgCom::bulid()->item;
    mItem->cnt = 0;
    initLayout(); isStart = false;
    CfgCom::bulid()->readMaxMinData();
    timer = new QTimer(this); //timer->start(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutDone()));
    QTimer::singleShot(1715,this,SLOT(updateWidSlot()));
    connect(modthread, &Modbus_Thread::overSig, this, &Home_WorkWid::updateResult);
    connect(modthread, &Modbus_Thread::msgSig, this, &Home_WorkWid::insertTextSlot);
    connect(modthread, &Modbus_Thread::finshSig, this, &Home_WorkWid::finishSlot);
}
void Home_WorkWid::initData()
{
    float temp ,mtemp;

    mItem->Min[0] = ui->powMinSpinBox->value() * 1000;
    mItem->Min[1] = ui->volMinSpinBox_2->value() * 1000;
    mItem->Min[2] = ui->volMinSpinBox_3->value() * 1000;
    mItem->Min[3] = ui->volMinSpinBox_4->value() * 1000;
    mItem->Min[4] = ui->volMinSpinBox_5->value() * 1000;

    mItem->Max[0] = ui->powMaxSpinBox->value() * 1000;
    mItem->Max[1] = ui->volMaxSpinBox_2->value() * 1000;
    mItem->Max[2] = ui->volMaxSpinBox_3->value() * 1000;
    mItem->Max[3] = ui->volMaxSpinBox_4->value() * 1000;
    mItem->Max[4] = ui->volMaxSpinBox_5->value() * 1000;

    for(int i = 0;i<5;i++)
    {
        temp = mItem->Min[i] / 1000.0;
        mtemp = mItem->Max[i] / 1000.0;
        if(i == 0){
            mItem->nolmin[i] = QString::number(temp,'f',2);
            mItem->nolmax[i] = QString::number(mtemp,'f',2);
        }else{
            mItem->nolmin[i] = QString::number(temp,'f',3);
            mItem->nolmax[i] = QString::number(mtemp,'f',3);
        }
    }

    CfgCom::bulid()->initMaxMinData();

}

void Home_WorkWid::logWrite()
{
    sLogItem logIt;
    QString str = tr("p1 = %1W，v2 = %2V，v3 = %3V，v4 = %4V，v5 = %5V").arg(mItem->noldata[0])
                      .arg(mItem->noldata[1]).arg(mItem->noldata[2]).arg(mItem->noldata[3]).arg(mItem->noldata[4]);
    logIt.sn = ui->snEdit->text();
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
    updateCntSlot();logWrite();
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
        mflag = false;
        str = tr("失败"); style = "background-color:red; color:rgb(255, 255, 255);";
    } style += "font:100 34pt \"微软雅黑\";";

    isStart = false;
    setWidEnabled(true);
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
    if(ret) ret = updateWid();
    if(ret) {
        mResult = true;
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

void Home_WorkWid::updateWidSlot()
{
    updateWid();
}

bool Home_WorkWid::updateWid()
{
    bool ret = true;
    ui->powMinSpinBox->setValue(mItem->nolmin[0].toDouble());
    ui->powMaxSpinBox->setValue(mItem->nolmax[0].toDouble());

    ui->volMinSpinBox_2->setValue(mItem->nolmin[1].toDouble());
    ui->volMinSpinBox_3->setValue(mItem->nolmin[2].toDouble());
    ui->volMinSpinBox_4->setValue(mItem->nolmin[3].toDouble());
    ui->volMinSpinBox_5->setValue(mItem->nolmin[4].toDouble());

    ui->volMaxSpinBox_2->setValue(mItem->nolmax[1].toDouble());
    ui->volMaxSpinBox_3->setValue(mItem->nolmax[2].toDouble());
    ui->volMaxSpinBox_4->setValue(mItem->nolmax[3].toDouble());
    ui->volMaxSpinBox_5->setValue(mItem->nolmax[4].toDouble());

    return ret;
}


void Home_WorkWid::timeoutDone()
{
    updateTime();
    initData();
}

void Home_WorkWid::on_startBtn_clicked()
{
    bool ret = true;
    if(isStart == false) {
        // if(!ui->snEdit->text().isEmpty())
        // {
            if(initWid()){
                mflag = true;
                timer->start(500);
                modthread->start();
            }
        // }else MsgBox::critical(this, tr("请先填写PCB码！"));
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
    initData();
}
void Home_WorkWid::on_savebtn_clicked()
{
    static int flg = 0;
    QString str = tr("修改");
    bool ret = usr_land_jur();
    if(!ret) {
        MsgBox::critical(this, tr("你无权进行此操作"));
        return;
    }

    if(flg++ %2) ret = false;
    else str = tr("保存");

    ui->savebtn->setText(str);
    ui->powMinSpinBox->setEnabled(ret);
    ui->volMinSpinBox_2->setEnabled(ret);
    ui->volMinSpinBox_3->setEnabled(ret);
    ui->volMinSpinBox_4->setEnabled(ret);
    ui->volMinSpinBox_5->setEnabled(ret);

    ui->powMaxSpinBox->setEnabled(ret);
    ui->volMaxSpinBox_2->setEnabled(ret);
    ui->volMaxSpinBox_3->setEnabled(ret);
    ui->volMaxSpinBox_4->setEnabled(ret);
    ui->volMaxSpinBox_5->setEnabled(ret);

    QTimer::singleShot(50,this,SLOT(saveFunSlot()));
}
void Home_WorkWid::on_snEdit_textChanged(const QString &arg1)
{
    ui->snEdit->setClearButtonEnabled(1);
}
