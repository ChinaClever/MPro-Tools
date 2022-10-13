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

    if(!QFile::exists("gzipFile.exe")) {
        MsgBox::critical(this, tr("缺少压缩程序 gzipFile.exe ！！"));
        return false;
    }

    return true;
}

void Home_WorkWid::writeLog(const sAppVerIt &app)
{
    sLogItem it;
    it.fn =  ui->pathEdit->text();
    it.sw = app.ver;
    it.md5 = app.md5;
    it.user = app.usr;
    it.remark = app.remark;
    it.old = app.oldVersion;
    DbLogs::bulid()->insertItem(it);
}

bool Home_WorkWid::packing(Cfg_App &cfg, const QStringList &apps)
{
    sAppVerIt it; it.apps = apps;
    it.usr = ui->usrEdit->text();
    it.ver = ui->versionEdit->text();
    it.remark = ui->textEdit->toPlainText();
    it.oldVersion = ui->oldVerEdit->text();
    it.releaseDate = QDate::currentDate().toString("yyyy-MM-dd");
    QString str = it.usr + it.ver + it.remark + it.oldVersion + it.releaseDate;
    it.md5 = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5).toHex();
    bool ret = cfg.app_pack(it);
    if(ret) writeLog(it);
    return ret;
}

bool Home_WorkWid::startZip()
{
    QString dir = ui->pathEdit->text();
    QString cmd = "gzipFile.exe ";
    cmd += ui->pathEdit->text();
    QProcess pro; pro.start(cmd);
    MsgBox::information(this, tr("正在打包软件，请等待压缩完成！！\n%1").arg(dir));
    pro.waitForFinished();
    QByteArray bs = pro.readAllStandardOutput();
    bs +=  pro.readAllStandardError();
    QString str = QString::fromLocal8Bit(bs);
    if(str.size()) {
        emit msgSig(str);
        File::AppendMd5(dir+".zip");
    }

    return str.size();
}

bool Home_WorkWid::workDown()
{    
    QString dir = ui->pathEdit->text();
    Cfg_App cfg(dir, this); emit startSig();
    QStringList fs = File::entryList(dir+"/app");
    fs.removeOne(CFG_APP); packing(cfg, fs);
    if(fs.size()) emit downSig(dir);
    return fs.size();
}


void Home_WorkWid::on_startBtn_clicked()
{
    bool ret = checkInput();
    if(ret) {
        ret = workDown();
        if(ret) {
            if(startZip())MsgBox::information(this, tr("打包发布完成！"));
            else MsgBox::critical(this, tr("压缩错误，未正常打包软件！！"));
        } else  MsgBox::critical(this, tr("发布错误，未发现文件！！"));
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
    bool ret = false;
    QString dir = ui->pathEdit->text();
    Cfg_App cfg(dir, this); emit downSig(dir);
    sAppVerIt it; ret = cfg.app_unpack(it);

    if(ret) MsgBox::information(this, tr("验证成功！！"));
    else MsgBox::critical(this, tr("验证错误，%1"));
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


