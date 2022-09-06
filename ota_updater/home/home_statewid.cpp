/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_statewid.h"
#include "ui_home_statewid.h"


Home_StateWid::Home_StateWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_StateWid)
{
    ui->setupUi(this);  mCnt = 0;
    QTimer::singleShot(1350,this,SLOT(initFunSlot()));
    QGridLayout *gridLayout = new QGridLayout(parent);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(this);
}

Home_StateWid::~Home_StateWid()
{
    delete ui;
}

void Home_StateWid::initFunSlot()
{
    Core_Sender *sender = Core_Sender::bulid(this);
    connect(sender, &Core_Sender::throwSig, this, &Home_StateWid::insertSlot);
    connect(sender, &Core_Sender::errorMessage, this, &Home_StateWid::insertSlot);
}

void Home_StateWid::startSlot()
{
    ui->textEdit->clear();  mCnt = 0;
}

void Home_StateWid::insertSlot(const QString &str)
{
    if(str.size()>2) ui->textEdit->appendPlainText(str);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
    if(mCnt++ > 500) ui->textEdit->clear();
}


