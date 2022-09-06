/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "pro_resultwid.h"
#include "ui_pro_resultwid.h"

Pro_ResultWid::Pro_ResultWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Pro_ResultWid)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    QTimer::singleShot(150,this,SLOT(initFunSlot()));
}

Pro_ResultWid::~Pro_ResultWid()
{
    delete ui;
}

void Pro_ResultWid::initFunSlot()
{
    Core_Sender *sender = Core_Sender::bulid(this);
    connect(sender, &Core_Sender::finishSig, this, &Pro_ResultWid::finishSlot);
}

void Pro_ResultWid::startSlot()
{
    ui->errTextEdit->clear();
    ui->okTextEdit->clear();
}

void Pro_ResultWid::finishSlot(bool pass, const QString &ip)
{
    QPlainTextEdit *t = nullptr;
    if(pass) t = ui->okTextEdit;
    else t = ui->errTextEdit;
    insertText(t, ip);
}

void Pro_ResultWid::insertText(QPlainTextEdit *textEdit, const QString &str)
{
    textEdit->moveCursor(QTextCursor::Start);
    textEdit->insertPlainText(str+"\n");
}
