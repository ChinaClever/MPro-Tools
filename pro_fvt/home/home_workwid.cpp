/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QStandardPaths>
#include <QFileDialog>
#include "filemgr.h"

Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this);
}

Home_WorkWid::~Home_WorkWid()
{
    delete ui;
}

void Home_WorkWid::on_startBtn_clicked()
{

}

void Home_WorkWid::createDir()
{
    FileMgr::build().createDir();
}
