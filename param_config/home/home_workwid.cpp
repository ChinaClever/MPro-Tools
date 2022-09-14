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
    ui->setupUi(this);
    mSsdp = Ssdp_Core::bulid(this);
}

Home_WorkWid::~Home_WorkWid()
{
    delete ui;
}


void Home_WorkWid::on_findBtn_clicked()
{
    QStringList ips = mSsdp->searchAll();
    QString str = tr("未找到任何目标设备");
    if(ips.size()) {
        str = tr("已找到 %1 目标IP:\n").arg(ips.size());
        int i=0; foreach(const auto &ip, ips) {
            str += "   " + ip; i++;
            if(i%3==0) str += "\n";
        }
    }
    MsgBox::information(this, str);
}


void Home_WorkWid::on_selectBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->pathEdit->setText(dir);
    ui->groupBox_2->setEnabled(dir.size());
    ui->groupBox_3->setEnabled(dir.size());
}

void Home_WorkWid::on_rangeBox_currentIndexChanged(int index)
{
    bool en = false;
    if(index) en = true;
    ui->dstEdit->setEnabled(en);
}

