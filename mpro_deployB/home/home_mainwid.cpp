/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_mainwid.h"
#include "ui_home_mainwid.h"
#include <QFileDialog>

Home_MainWid::Home_MainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_MainWid)
{
    ui->setupUi(this); //initWid();
    groupBox_background_icon(this); mSsdp = Ssdp_Core::bulid(this);
    connect(mSsdp, &Ssdp_Core::sendMsgSig, this, &Home_MainWid::onDown);
    // timer = new QTimer(this); //timer->start(500);
    // connect(timer, SIGNAL(timeout()), this, SLOT(timeoutDone()));
    groupBox_background(ui->tab_5);
    groupBox_background(ui->tab_6);
    disable(0);
    setWorkDown();
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}

void Home_MainWid::webLogin()
{
    sCfgItem it; it.type = 14;
    QString v = ui->usrEdit->text();
    if(v.size()) {
        it.fc = 1;
        send(it, v);
    }

    v = ui->pwdEdit->text();
    if(v.size()) {
        it.fc =2;
        send(it, v);
    }
}

void Home_MainWid::initWid()
{
    QPalette pl = ui->textEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
}


void Home_MainWid::onDown(const QString &msg)
{
    QString str = QString::number(++mId) + "、"+ msg + "\n";
    ui->textEdit->moveCursor(QTextCursor::Start);
    ui->textEdit->insertPlainText(str);
}

void Home_MainWid::on_findBtn_clicked()
{
    QStringList ips = mSsdp->searchAll();
    QString str = tr("未找到任何目标设备");
    if(ips.size()) {
        str = tr("已找到%1个设备，包含%2个主机，%3个副机\t\n")
                  .arg(ips.size() + mSsdp->getSlaveNum())
                  .arg(ips.size()).arg(mSsdp->getSlaveNum());
        int i=0; foreach(const auto &ip, ips) {
            str += "   " + ip; i++;
            if(i%4==0) str += "\n";
        }
    }
    MsgBox::information(this, str);
}

void Home_MainWid::send(const sCfgItem &it, const QVariant &v)
{
    QString room, ip; //getInput(room, ip);
    if(ui->checkBox->isChecked()) ip = ui->dstEdit->text();
    mSsdp->setCfg(it, v, room, ip);
}
void Home_MainWid::on_startBtn_clicked()
{
    ui->textEdit->clear(); mId = 0;
    bool ret = cm_pingNet(ui->dstEdit->text());
    if(ret){
        if(ui->webCheckBox->isChecked()) webLogin();
        if(ui->netcheckBox->isChecked()) netAddr();
        if(ui->tcpcheckBox->isChecked()) {devMode(); modbusTcp();}//tcp
        if(ui->snmpcheckBox->isChecked()) {devMode(); snmp();}
        if(ui->rtucheckBox->isChecked()) modbusRtu();//rtu

        setDateTime(); reboot(); incrementAddr();
    }else MsgBox::critical(this,  "目标设备不存在！");

}

void Home_MainWid::devMode()
{
    int v = ui->modeBox->currentIndex();
        sCfgItem it; if(v) {
            it.type = 13;
            it.fc = 3;
            send(it, v);

            it.fc = 4;
            v = ui->addrBox->currentIndex(); send(it, v);
            if(v) ui->comboBox->setEnabled(0);
        } else {
            it.type = 15;
            it.fc = 1;
            send(it, 1);
            it.fc = 2;
            send(it, ui->rtu_addrspinBox->value());
            it.fc = 3;
            send(it, ui->rtubaudBox->currentText());
            it.fc = 7;
            send(it, 1);
        }
}
void Home_MainWid::modbusTcp()
{
    sCfgItem it;
    it.type = 15; it.fc = 11;//Modbus TCP设置
    int v = ui->tcpBox->currentIndex();
    send(it,v);
    it.fc = 12;
    send(it,ui->tcpportEdit->text());
    it.fc = 14;send(it,1);
}
void Home_MainWid::snmp()
{
    sCfgItem it;
    int tem = ui->snmpBox->currentIndex();
    if(tem == 0){//禁用v2/v3
        disable(tem);
        it.type = 16; it.fc = 1;
        int v = ui->snmpBox->currentIndex();
        send(it,v);

        it.type = 16; it.fc = 11;
        send(it,v);
        it.fc = 17;send(it,0);

    }else if(tem == 1){//启用v2
        v2_able();
        it.type = 16; it.fc = 1;
        send(it,1);

        it.type = 16; it.fc = 2;
        send(it,ui->v2_getEdit->text());

        it.type = 16; it.fc = 3;
        send(it,ui->v2_setEdit->text());
        it.fc = 17;send(it,1);

    }else if(tem == 2){//启用v3
        v3_able();
        it.type = 16; it.fc = 11;
        send(it,1);

        it.type = 16; it.fc = 12;
        send(it,ui->v3_Edit->text());

        it.type = 16; it.fc = 13;
        send(it,ui->v3_pwdEdit->text());

        it.type = 16; it.fc = 14;
        send(it,ui->v3_keyEdit->text());
        it.fc = 17;send(it,1);

    }
    QString tr;
    it.fc = 21; it.id =1;
    tr = ui->trap1Edit->text();
    if(tr.size()) send(it,tr);

    it.fc = 21; it.id = 2;
    tr = ui->trap2Edit->text();
    send(it,tr);
}
void Home_MainWid::modbusRtu()
{
    sCfgItem it;
    int v = 0;
    it.type = 15; it.fc = 1;
    v = ui->rtuBox->currentIndex();
    send(it,v);

    it.fc = 2;
    send(it,ui->rtu_addrspinBox->value());

    it.fc = 3;
    v = ui->rtubaudBox->currentIndex();
    send(it,v);

    it.fc = 7;send(it,1);
}
void Home_MainWid::incrementAddr()
{
    int mode = ui->modeBox->currentIndex();
    if(mode && ui->cascheckBox->isChecked()) {
        int v = ui->addrBox->currentIndex();
        v +=1; if(mode ==1 && v>=10) {
            v = 0; if(ui->ipcheckBox->isChecked())incrementIPAddress();
        } ui->addrBox->setCurrentIndex(v);
    } else {if(ui->ipcheckBox->isChecked())incrementIPAddress();}

    mode = ui->rtuBox->currentIndex();
    if(ui->rtu_addrcheckBox->isChecked() && mode)
    {
        int t = ui->rtu_addrspinBox->value();
        t +=1;if(t >99) t = 0;
        ui->rtu_addrspinBox->setValue(t);
    }
}

void Home_MainWid::incrementIPAddress()
{
    QString ipAddress = ui->ipEdit->text();
    QHostAddress address(ipAddress);

    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        quint32 ipv4 = address.toIPv4Address();
        ipv4 += 1;
        address.setAddress(ipv4);
    } else if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        Q_IPV6ADDR ipv6 = address.toIPv6Address();
        // 加 1 操作
        for (int i = 15; i >= 0; --i) {
            if (ipv6[i] < 255) {
                ipv6[i] += 1;
                break;
            } else {
                ipv6[i] = 0;
            }
        }
        address.setAddress(ipv6);
    } else {
        // 不支持的地址类型
    }

    ipAddress = address.toString();
    ui->ipEdit->setText(ipAddress.remove("::ffff:"));
}
void Home_MainWid::reboot()
{
    sCfgItem it; it.type = 111;
    QString cmd = "reboot";
    cm_mdelay(655); send(it, cmd);
}

void Home_MainWid::setDateTime()
{
    sCfgItem it; it.type = 43;  it.fc = 1;
    QDateTime dateTime = QDateTime::currentDateTime();
    QString v = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    send(it, v);
}

// void Home_MainWid::location()
// {
//     sCfgItem it; it.type = 11;
//     QString v = ui->roomEdit->text();
//     if(v.size()) {
//         it.fc = 1;
//         send(it, v);
//     }

//     v = ui->locatEdit->text();
//     if(v.size()) {
//         it.fc = 2;
//         send(it, v);
//     }
// }

void Home_MainWid::netAddr()
{
   sCfgItem it; it.type = 41; it.fc = 1;
    int id = ui->dhcpBox->currentIndex();
    send(it, id); if(id == 0) {
        QString v = ui->ipEdit->text();
        if(v.size()) {it.fc = 2; send(it, v);}

        v = ui->maskEdit->text();
        if(v.size()) {it.fc = 3; send(it, v);}

        v = ui->gwEdit->text();
        if(v.size()) {it.fc = 4; send(it, v);}

        v = ui->dnsEdit->text();
        if(v.size()) {it.fc = 6; send(it, v);}

        it.fc = 15; send(it, 1);
    }
}


QByteArray Home_MainWid::readFile(const QString &fn)
{
    QFile file(fn); QByteArray res;
    bool ret = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(ret) res = file.readAll();
    else cout << file.errorString();
    file.close();
    return res;
}

bool Home_MainWid::checkFile(const QByteArray &msg)
{
    QJsonObject obj;
    QJsonParseError jsonerror; bool ret = false;
    QJsonDocument doc = QJsonDocument::fromJson(msg, &jsonerror);
    if (!doc.isNull() && jsonerror.error == QJsonParseError::NoError) {
        if(doc.isObject()) {
            obj = doc.object();
            ret = true;
        }
    }
    return ret;
}


void Home_MainWid::setWorkDown()
{
    // QString dir = ui->pathEdit->text();
    // QString room, ip; //getInput(room, ip);
    // if(ui->checkBox->isChecked()) ip = ui->dstEdit->text();
    // if(dir.size() && ui->batchCheck->isChecked()) {
    //     QStringList ls = File::entryList(dir);
    //     foreach(const auto &f, ls) {
    //         QString fn = dir + "/" + f;
    //         QByteArray res = readFile(fn);
    //         if(checkFile(res)) mSsdp->setJson(res, room, ip);
    //     }
    // }

    ui->batchCheck->hide();ui->selectBtn->hide();ui->pathEdit->hide();
    ui->usrEdit->setEnabled(0); ui->pwdEdit->setEnabled(0);

    ui->dhcpBox->setEnabled(0); ui->dnsEdit->setEnabled(0);
    ui->gwEdit->setEnabled(0); ui->ipEdit->setEnabled(0);ui->maskEdit->setEnabled(0);

    ui->tcpBox->setEnabled(0); ui->tcpportEdit->setEnabled(0);

    ui->trap1Edit->setEnabled(0);ui->trap2Edit->setEnabled(0);
    ui->snmpBox->setEnabled(0);ui->v2_getEdit->setEnabled(0);
    ui->v2_setEdit->setEnabled(0);ui->v3_Edit->setEnabled(0);
    ui->v3_keyEdit->setEnabled(0);ui->v3_pwdEdit->setEnabled(0);

    ui->rtuBox->setEnabled(0); ui->rtu_addrspinBox->setEnabled(0);ui->rtubaudBox->setEnabled(0);
}

void Home_MainWid::on_dhcpBox_currentIndexChanged(int index)
{
    ui->ipEdit->setDisabled(index);
    ui->maskEdit->setDisabled(index);
    ui->gwEdit->setDisabled(index);
    ui->dnsEdit->setDisabled(index);
}

// void Home_MainWid::on_pushBox_currentIndexChanged(int index)
// {
//     bool en = false; if(index == 1) en = true;
//     ui->hostEdit->setEnabled(index);
//     ui->portEdit->setEnabled(en);
// }


void Home_MainWid::on_modeBox_currentIndexChanged(int index)
{
    if(ui->rtuBox->currentIndex()){
        if(index == 1){
            ui->rtuBox->setCurrentIndex(0);
            ui->modeBox->setCurrentIndex(1);
            ui->rtu_addrspinBox->setEnabled(0);
            ui->rtubaudBox->setEnabled(0);
            ui->addrBox->setEnabled(1);
            ui->comboBox->setEnabled(1);
        }
    }else{
        ui->addrBox->setEnabled(index);
        ui->comboBox->setEnabled(index);
    }
}


void Home_MainWid::on_checkBox_stateChanged(int arg1)
{
    ui->dstEdit->setEnabled(arg1);
}

void Home_MainWid::on_batchCheck_clicked(bool checked)
{
    ui->pathEdit->setEnabled(checked);
    ui->selectBtn->setEnabled(checked);
}

void Home_MainWid::on_selectBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->pathEdit->setText(dir);
}

void Home_MainWid::setEnableWid(bool en)
{
    ui->groupBox_1->setEnabled(en);
    ui->groupBox_2->setEnabled(en);
    ui->groupBox_3->setEnabled(en);
    ui->groupBox_4->setEnabled(en);
    ui->tabWidget->setEnabled(en);
}


// void Home_MainWid::on_rangeBox_currentIndexChanged(int index)
// {
//     if(index) {
//         setEnableWid(false); switch (index) {
//         case 1: ui->groupBox_1->setEnabled(true); break;
//         case 2: ui->groupBox_2->setEnabled(true); break;
//         case 3: ui->groupBox_3->setEnabled(true); break;
//         case 4: ui->groupBox_4->setEnabled(true); break;
//         case 5: ui->tabWidget->setEnabled(true); break;
//         }
//     } else setEnableWid(true);
// }

void Home_MainWid::on_snmpBox_currentIndexChanged(int index)
{

    if(index == 0){//禁用v2/v3
        disable(index);
    }else if(index == 1){//启用v2
        v2_able();
    }else if(index == 2){//启用v3
        v3_able();
    }
}

void Home_MainWid::on_rtuBox_currentIndexChanged(int index)
{
    if(ui->modeBox->currentIndex()){
        if(index == 1){
            ui->modeBox->setCurrentIndex(0);
            ui->rtuBox->setCurrentIndex(1);
            ui->addrBox->setEnabled(0);
            ui->comboBox->setEnabled(0);
            ui->rtu_addrspinBox->setEnabled(1);
            ui->rtubaudBox->setEnabled(1);
        }
    }else{
        ui->rtu_addrspinBox->setEnabled(index);
        ui->rtubaudBox->setEnabled(index);
    }
}

void Home_MainWid::on_tcpBox_currentIndexChanged(int index)
{
    ui->tcpportEdit->setEnabled(index);
    ui->tcpportEdit->setEnabled(index);
}

void Home_MainWid::disable(bool v)
{
    ui->v2_getEdit->setEnabled(v);
    ui->v2_setEdit->setEnabled(v);

    ui->v3_Edit->hide();
    ui->v3_keyEdit->hide();
    ui->v3_pwdEdit->hide();
    ui->label_44->hide();
    ui->label_45->hide();
    ui->label_46->hide();
    ui->label_47->hide();

}
void Home_MainWid::v2_able()//启用v2
{
    ui->v2_getEdit->setEnabled(1);
    ui->v2_setEdit->setEnabled(1);

    ui->v3_Edit->show();
    ui->v3_keyEdit->show();
    ui->v3_pwdEdit->show();

    ui->v3_Edit->setEnabled(0);
    ui->v3_keyEdit->setEnabled(0);
    ui->v3_pwdEdit->setEnabled(0);

    ui->label_44->show();
    ui->label_45->show();
    ui->label_46->show();
    ui->label_47->show();
}
void Home_MainWid::v3_able()
{
    ui->v3_Edit->show();
    ui->v3_keyEdit->show();
    ui->v3_pwdEdit->show();

    ui->v2_getEdit->setEnabled(0);
    ui->v2_setEdit->setEnabled(0);

    ui->v3_Edit->setEnabled(1);
    ui->v3_keyEdit->setEnabled(1);
    ui->v3_pwdEdit->setEnabled(1);
}

void Home_MainWid::on_addrBox_currentIndexChanged(int index)
{
    if(index){ui->comboBox->hide(); ui->label_20->hide();}
    else{ui->comboBox->show(); ui->label_20->show();}
}
void Home_MainWid::on_webCheckBox_stateChanged(int arg1)
{
    if(arg1) {ui->usrEdit->setEnabled(1); ui->pwdEdit->setEnabled(1);}
    else {ui->usrEdit->setEnabled(0); ui->pwdEdit->setEnabled(0);}
}

void Home_MainWid::on_netcheckBox_stateChanged(int arg1)
{
    if(arg1) {ui->dhcpBox->setEnabled(1); ui->dnsEdit->setEnabled(1);
        ui->gwEdit->setEnabled(1); ui->ipEdit->setEnabled(1);ui->maskEdit->setEnabled(1);}
    else {ui->dhcpBox->setEnabled(0); ui->dnsEdit->setEnabled(0);
        ui->gwEdit->setEnabled(0); ui->ipEdit->setEnabled(0);ui->maskEdit->setEnabled(0);}
}

void Home_MainWid::on_tcpcheckBox_stateChanged(int arg1)
{
    if(arg1) {ui->tcpBox->setEnabled(1); ui->tcpportEdit->setEnabled(1);}
    else {ui->tcpBox->setEnabled(0); ui->tcpportEdit->setEnabled(0);}
}

void Home_MainWid::on_snmpcheckBox_stateChanged(int arg1)
{
    if(arg1){ui->trap1Edit->setEnabled(1);ui->trap2Edit->setEnabled(1);
        ui->snmpBox->setEnabled(1);ui->v2_getEdit->setEnabled(1);
        ui->v2_setEdit->setEnabled(1);ui->v3_Edit->setEnabled(1);
        ui->v3_keyEdit->setEnabled(1);ui->v3_pwdEdit->setEnabled(1);}
    else {ui->trap1Edit->setEnabled(0);ui->trap2Edit->setEnabled(0);
        ui->snmpBox->setEnabled(0);ui->v2_getEdit->setEnabled(0);
        ui->v2_setEdit->setEnabled(0);ui->v3_Edit->setEnabled(0);
        ui->v3_keyEdit->setEnabled(0);ui->v3_pwdEdit->setEnabled(0);}
}

void Home_MainWid::on_rtucheckBox_stateChanged(int arg1)
{
    if(arg1) {ui->rtuBox->setEnabled(1); ui->rtu_addrspinBox->setEnabled(1);ui->rtubaudBox->setEnabled(1);}
    else {ui->rtuBox->setEnabled(0); ui->rtu_addrspinBox->setEnabled(0);ui->rtubaudBox->setEnabled(0);}
}

