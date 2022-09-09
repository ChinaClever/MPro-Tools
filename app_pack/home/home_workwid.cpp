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
    ui->checkBox->setHidden(true);
}

Home_WorkWid::~Home_WorkWid()
{
    delete ui;
}

bool Home_WorkWid::checkInput()
{
    QString str = ui->pathEdit->text();
    if(str.isEmpty()) {
        MsgBox::critical(this, tr("目录没有选择，请选择目录！！"));
        return false;
    }

    str = ui->usrEdit->text();
    if(str.isEmpty()) {
        MsgBox::critical(this, tr("客户名称不能为空！！"));
        return false;
    }

    str = ui->oldVerEdit->text();
    if(str.isEmpty()) {
        MsgBox::critical(this, tr("依赖的版号不能为空！！"));
        return false;
    }

    str = ui->versionEdit->text();
    if(str.isEmpty()) {
        MsgBox::critical(this, tr("软件版号不能为空！！"));
        return false;
    }

    str = ui->textEdit->toPlainText();
    if(str.size() < 15) {
        MsgBox::critical(this, tr("发布说明信息太过简单！！"));
        return false;
    }
    return true;
}

void Home_WorkWid::writeLog(const sAppVerIt &app)
{
    sLogItem it;
    it.fn = app.app;
    it.sw = app.ver;
    it.md5 = app.md5;
    it.user = app.usr;
    it.old = app.oldVersion;
    it.remark = app.remark;
    DbLogs::bulid()->insertItem(it);
}

bool Home_WorkWid::packing(Cfg_App &cfg, const QString &fn)
{
    sAppVerIt it; it.app = fn;
    it.usr = ui->usrEdit->text();
    it.ver = ui->versionEdit->text();
    it.remark = ui->textEdit->toPlainText();
    it.oldVersion = ui->oldVerEdit->text();
    bool ret = cfg.app_pack(it);
    if(ret) writeLog(it);
    return ret;
}

bool Home_WorkWid::workDown()
{    
    QString dir = ui->pathEdit->text();
    QStringList fs = File::entryList(dir);
    Cfg_App cfg(dir, this); emit startSig();
    QString res; foreach(const auto &fn, fs) {
        if(fn == CFG_APP) continue;
        if(packing(cfg, fn)) res += "\n\t" + fn;
    }

    if(fs.size()) MsgBox::information(this, tr("发布成功！！ %1").arg(res));
    else  MsgBox::critical(this, tr("发布错误，未发现文件！！"));
    return fs.size();
}


void Home_WorkWid::on_startBtn_clicked()
{
    bool ret = checkInput();
    if(ret) {
        ret = workDown();
        if(ret) {
            QString dir = ui->pathEdit->text();
            emit downSig(dir);
        }
    }
}

void Home_WorkWid::on_seeBtn_clicked()
{
    QString dir = ui->pathEdit->text();
    if(QFile::exists(dir +"/" + CFG_APP)) {
        emit downSig(dir);
        MsgBox::information(this, tr("查看成功！！"));
    } else MsgBox::critical(this, tr("查看错误，未发现记录文件！！"));
}


void Home_WorkWid::on_checkBtn_clicked()
{
    bool ret = false; QString res, ok;
    QString dir = ui->pathEdit->text();
    Cfg_App cfg(dir, this); emit downSig(dir);
    QStringList fs = File::entryList(dir);
    foreach(const auto &fn, fs) {
        if(fn == CFG_APP) continue;
        ret = cfg.app_check(fn);
        if(ret)ok += "\n\t" + fn;
        else res += "\n\t" + fn;
    }

    if(res.size()) {
        MsgBox::critical(this, tr("验证错误，%1").arg(res));
    }else MsgBox::information(this, tr("验证成功！！").arg(ok));
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


