/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "log_logquerydlg.h"
#include "ui_log_logquerydlg.h"

Log_LogQueryDlg::Log_LogQueryDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log_LogQueryDlg)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("查询对话框"));
    groupBox_background_icon(this);
    mDateBar = new SqlDateBar(ui->dateWid);
}

Log_LogQueryDlg::~Log_LogQueryDlg()
{
    delete ui;
}

QString Log_LogQueryDlg::getCmd()
{
    QString cmd = mDateBar->getDate();
    QString str = ui->dstEdit->text();
    if(!str.isEmpty()) cmd += QString(" and dst like '%%1%'").arg(str);

    return cmd;
}


void Log_LogQueryDlg::on_quitBtn_clicked()
{
    close();
}

bool Log_LogQueryDlg::inputCheck()
{
    bool ret = true;
/*
    QString str = ui->md5Edit->text();
    if(!str.isEmpty()) {
        if(str.size() != 32) ret = false;
        if(!ret) {
            MsgBox::critical(this, tr("校验码输入有误，请重新输入!!!"));
        }
    }
*/
    return ret;
}

void Log_LogQueryDlg::on_okBtn_clicked()
{
    if(inputCheck()) {
        this->accept();
    }
}