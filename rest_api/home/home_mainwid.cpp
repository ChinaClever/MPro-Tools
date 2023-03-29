/*
 *
 *  Created on: 2022年1月1日
 *      Author: Lzy
 */
#include "home_mainwid.h"
#include "ui_home_mainwid.h"
#include "backcolourcom.h"
#include <QFileDialog>
#include "cfg_file.h"
#include "print.h"

Home_MainWid::Home_MainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_MainWid)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    connect(this, &Home_MainWid::httpSig, this, &Home_MainWid::onDown);
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}


void Home_MainWid::http_get(const QString &method, QJsonObject &json)
{
    QSslConfiguration SSLConfig;
    SSLConfig = QSslConfiguration::defaultConfiguration();
    SSLConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    SSLConfig.setProtocol(QSsl::TlsV1_2);

    QString url = "https://%1:%2/%3";
    QString ip = ui->ipEdit->text();
    int port = ui->portBox->value();
    mHttp.get(url.arg(ip).arg(port).arg(method))
            .header("content-type", "application/json")
            .onSuccess([&](QString result) {emit httpSig(result);})
    .onFailed([&](QString error) {emit httpSig(error); })
    .onTimeout([&](QNetworkReply *) {emit httpSig("timeout"); }) // 超时处理
    .sslConfiguration(SSLConfig)
            .timeout(1000) // 1s超时
            .body(json)
            .exec();
}


void Home_MainWid::http_post(const QString &method, QJsonObject &json)
{
    QSslConfiguration SSLConfig;
    SSLConfig = QSslConfiguration::defaultConfiguration();
    SSLConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    SSLConfig.setProtocol(QSsl::TlsV1_2);

    QString url = "https://%1:%2/%3";
    QString ip = ui->ipEdit->text();
    int port = ui->portBox->value();
    mHttp.post(url.arg(ip).arg(port).arg(method))
            .header("content-type", "application/json")
            .onSuccess([&](QString result) {emit httpSig(result);})
    .onFailed([&](QString error) {emit httpSig(error); })
    .onTimeout([&](QNetworkReply *) {emit httpSig("timeout"); }) // 超时处理
    .sslConfiguration(SSLConfig)
            .timeout(1000) // 1s超时
            .body(json)
            .exec();
}


void Home_MainWid::http_put(const QString &method, QJsonObject &json)
{
    QSslConfiguration SSLConfig;
    SSLConfig = QSslConfiguration::defaultConfiguration();
    SSLConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    SSLConfig.setProtocol(QSsl::TlsV1_2);

    QString url = "https://%1:%2/%3";
    QString ip = ui->ipEdit->text();
    int port = ui->portBox->value();
    mHttp.put(url.arg(ip).arg(port).arg(method))
            .header("content-type", "application/json")
            .onSuccess([&](QString result) {emit httpSig(result);})
    .onFailed([&](QString error) {emit httpSig(error); })
    .onTimeout([&](QNetworkReply *) {emit httpSig("timeout"); }) // 超时处理
    .sslConfiguration(SSLConfig)
            .timeout(1000) // 1s超时
            .body(json)
            .exec();
}

void Home_MainWid::on_alrmBtn_clicked()
{
    QJsonObject json, obj;
    json.insert("addr",  ui->addrSpinBox->value());
    json.insert("type", ui->alarmTypeBox->value());
    json.insert("topic", ui->alrmTopicBox->value());
    json.insert("subtopic", ui->alarmSubBox->value());
    json.insert("id", ui->alarmIdBox->value());
    json.insert("value", ui->alarmValueBox->value());
    int index =  ui->alarmBox->currentIndex();
    if(index) {
        obj.insert("pduDataSet", json);
        http_post("pduSetting", obj);
    } else {
        obj.insert("pduDataGet", json);
        http_get("pduGetting", obj);
    }
}

void Home_MainWid::on_cfgBtn_clicked()
{
    QJsonObject json, obj;
    json.insert("addr", ui->addrSpinBox->value());
    json.insert("type", ui->cfgTypeBox->value());
    json.insert("fc", ui->cfgFcBox->value());
    json.insert("id", ui->cfgIdBox->value());
    json.insert("value", ui->cfgEdit->text());
    int index =  ui->cfgBox->currentIndex();
    if(index) {
        obj.insert("pduCfgSet", json);
        http_post("pduSetting", obj);
    } else {
        obj.insert("pduCfgGet", json);
        http_get("pduGetting", obj);
    }
}

void Home_MainWid::on_swBtn_clicked()
{
    QJsonObject json, obj;
    json.insert("topic", 1);
    json.insert("subtopic", 11);
    json.insert("addr",  ui->addrSpinBox->value());
    json.insert("type", ui->startBox->value());
    json.insert("id", ui->numBox->value());
    json.insert("value", ui->onBox->value());
    obj.insert("pduDataSet", json);
    http_post("pduSetting", obj);
}

void Home_MainWid::on_cmdBtn_clicked()
{
    QJsonObject json;
    json.insert("cmd", ui->cmdEdit->text());
    http_put("execute", json);
}

void Home_MainWid::on_readBtn_clicked()
{
    QJsonObject json;
    json.insert("dc", 3);
    json.insert("addr", ui->addrSpinBox->value());
    http_get("pduMetaData", json);
}

void Home_MainWid::on_alarmBox_currentIndexChanged(int index)
{
    ui->alarmValueBox->setEnabled(index);
}

void Home_MainWid::on_cfgBox_currentIndexChanged(int index)
{
    ui->cfgEdit->setEnabled(index);
}

void Home_MainWid::onDown(QString msg)
{
    ui->textEdit->clear();
    ui->textEdit->insertPlainText(msg);
}

void Home_MainWid::http_down(const QString &method, QJsonObject &json, const QString &file)
{
    QSslConfiguration SSLConfig;
    SSLConfig = QSslConfiguration::defaultConfiguration();
    SSLConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    SSLConfig.setProtocol(QSsl::TlsV1_2);

    QString url = "https://%1:%2/%3";
    QString ip = ui->ipEdit->text();
    int port = ui->portBox->value();
    mHttp.get(url.arg(ip).arg(port).arg(method))
            .header("content-type", "application/json")
            .download(file)  // 启用自动设置文件名字
            .sslConfiguration(SSLConfig)
            .onDownloadFileSuccess([&](QString fileName) { emit httpSig("Download completed: "+fileName);})
    .onDownloadFileFailed([&](QString error) { emit httpSig("Download failed: "+error); })
    .body(json)
            .exec();
}

void Home_MainWid::http_upload(const QString &method, const QString &file)
{
    QSslConfiguration SSLConfig;
    SSLConfig = QSslConfiguration::defaultConfiguration();
    SSLConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    SSLConfig.setProtocol(QSsl::TlsV1_2);

    QString url = "https://%1:%2/%3";
    QString ip = ui->ipEdit->text();
    int port = ui->portBox->value();
    mHttp.post(url.arg(ip).arg(port).arg(method))
            .body(file, file)
            .onSuccess([&](QString result) {emit httpSig(result);})
            .onFailed([&](QString error) {emit httpSig(error); })
            .sslConfiguration(SSLConfig)
            .exec();
}


void Home_MainWid::on_downBtn_clicked()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QJsonObject json; QString fn = ui->fileEdit->text(); json.insert("file", fn);
    QString file = dir + "/" + fn.split("/").last(); //qDebug() << file;
    http_down("download", json, file);
}


//void Home_MainWid::on_pushButton_clicked()
//{
//    http_upload("upload", "lzy.txt");
//}


void Home_MainWid::on_selectBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->batchEdit->setText(dir); ui->batchBtn->setEnabled(true);
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

bool Home_MainWid::checkInput(const QByteArray &msg, QJsonObject &obj)
{
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

void Home_MainWid::on_batchBtn_clicked()
{
    QString dir = ui->batchEdit->text();
    if(dir.size()) {
        QStringList ls = File::entryList(dir);
        foreach(const auto &f, ls) {
            QString fn = dir + "/" + f;
            QByteArray res = readFile(fn);
            QJsonObject obj;
            if(checkInput(res, obj)) {
                QString key = obj.keys().first();
                http_post(key, obj);
            }
        }
    }
}

bool Home_MainWid::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->textEdit) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            qDebug() << "Double click";
             return true;
        }
    }
    return false;
}
