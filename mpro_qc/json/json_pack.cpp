/*
 *
 *
 *  Created on: 2018年10月1日
 *      Author: Lzy
 */
#include "json_pack.h"


Json_Pack::Json_Pack(QObject *parent)
{
    mPro = sDataPacket::bulid()->getPro();
}

Json_Pack *Json_Pack::bulid(QObject *parent)
{
    static Json_Pack* sington = NULL;
    if(sington == NULL) {
        sington = new Json_Pack(parent);
    }
    return sington;
}


void Json_Pack::head(QJsonObject &obj)
{
    QDateTime t = QDateTime::currentDateTime();
    mPro->testEndTime = t.toString("yyyy-MM-dd HH:mm:ss");
    mPro->testTime = QString::number(QDateTime::fromString(mPro->testStartTime,"yyyy-MM-dd HH:mm:ss").secsTo(t));
    obj.insert("softwareType", mPro->softwareType);
    obj.insert("productType", mPro->productType);
    obj.insert("toolName","mPro_qc");
    obj.insert("moduleSn", mPro->moduleSn);
    mPro->productSN = "1234567890";
    obj.insert("productSN",mPro->productSN);
    obj.insert("macAddress", mPro->macAddress);
    obj.insert("languageSelect",0);
    obj.insert("result", mPro->uploadPassResult);
    obj.insert("softVersion", mPro->softwareVersion);
    obj.insert("companyName", mPro->companyName);
    obj.insert("protocolVersion", mPro->protocolVersion);
    obj.insert("testStartTime", mPro->testStartTime);
    obj.insert("testEndTime", mPro->testEndTime);
    obj.insert("testTime", mPro->testTime);
    obj.insert("orderId", mPro->pn);
    obj.insert("orderNum", mPro->orderNum);


    //pduInfo(obj,id);
}

void Json_Pack::pduInfo(QJsonObject &obj,const int id)
{
    objData(obj,id);
}


int Json_Pack::objData(QJsonObject &subObj,const int id)
{
 //   QJsonArray jsonArray;
 //   int num = mPro->uploadPass.size();
 //   qDebug()<<"aaaaaaaa"<<mPro->itemName.size()<<num;
 //   for(int i=0; i<num; ++i)
//    {
        //QJsonObject subObj;
        //subObj.insert("no",QString::number(i+1));
 qDebug()<<"hhhhhh";
        subObj.insert("testProcess", mPro->itemName.at(id));
        subObj.insert("testResult", mPro->uploadPass.at(id)?1:0);

        subObj.insert("testRequest", mPro->testRequest.at(id));
        subObj.insert("testStep", mPro->testStep.at(id));
        subObj.insert("testItem", mPro->testItem.at(id));


        //qDebug()<<subObj;
        //jsonArray.append(subObj);
 //   }
   // obj.insert("testStep" ,QJsonValue(jsonArray));

  //  return num;
        return 0;
}

void Json_Pack::http_post(const QString &method, QJsonObject &json, const int id, const QString &ip, int port)
{
    //QJsonObject json; head(json);

    pduInfo(json,id);
  //  qDebug()<<json;

    AeaQt::HttpClient http;
    http.clearAccessCache();
    http.clearConnectionCache();
    QString url = "http://%1:%2/%3";
    qDebug()<<"url: "<<url.arg(ip).arg(port).arg(method);
    http.post(url.arg(ip).arg(port).arg(method))
        .header("content-type", "application/json")
        .onSuccess([&](QString result) {emit httpSig(result,true,"","","");})
        .onFailed([&](QString error) {emit httpSig(error,false,"","",""); })
        .onTimeout([&](QNetworkReply *) {emit httpSig("http_post timeout",false,"","",""); }) // 超时处理
        .timeout(1000) // 1s超时
        .block()
        .body(json)
        .exec();
}

//bool Json_Build::saveJson( QJsonObject &json)
//{
//    QJsonDocument jsonDoc(json);
//    QByteArray ba = jsonDoc.toJson();
//    QString path = CfgCom::bulid()->pathOfData("pdu_id.json");
//    QFile file(path);
//    bool ret = false;
//    if(file.exists())//文件存在则不需要再写
//    {
//        ret = true;
//    }
//    else//文件不存在则写入初始数据
//    {
//        ret = file.open(QIODevice::WriteOnly);
//        if(ret) {
//            file.write(ba);
//            file.close();
//        } else {
//            qDebug() << "write json file failed";
//        }
//    }

//    return ret;
//}

