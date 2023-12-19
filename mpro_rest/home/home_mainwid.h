#ifndef HOME_MAINWID_H
#define HOME_MAINWID_H

#include <QWidget>
#include "http/httpclient.h"

namespace Ui {
class Home_MainWid;
}

class Home_MainWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_MainWid(QWidget *parent = nullptr);
    ~Home_MainWid();

signals:
    void httpSig(const QString &msg);


protected:
    virtual bool eventFilter(QObject * obj,QEvent *event) override;

private:
    void http_get(const QString &method, QJsonObject &json);
    void http_post(const QString &method, QJsonObject &json);
    void http_put(const QString &method, QJsonObject &json);
    void http_upload(const QString &method, const QString &file);
    void http_down(const QString &method, QJsonObject &json, const QString &file);
    bool checkInput(const QByteArray &msg, QJsonObject &obj);
    QByteArray readFile(const QString &fn);

private slots:
    void on_swBtn_clicked();
    void on_cfgBtn_clicked();
    void on_cmdBtn_clicked();
    void on_alrmBtn_clicked();
    void on_readBtn_clicked();
    void on_downBtn_clicked();
    void on_batchBtn_clicked();
    void on_selectBtn_clicked();
    //void on_pushButton_clicked();
    void on_cfgBox_currentIndexChanged(int index);
    void on_alarmBox_currentIndexChanged(int index);
    void onDown(QString msg);

private:
    Ui::Home_MainWid *ui;
    //AeaQt::HttpClient mHttp;
};

#endif // HOME_MAINWID_H
