
INCLUDEPATH += $$PWD

include(rpc/rpc.pri)
include(dtls/dtls.pri)
include(ssdp/ssdp.pri)
include(users/users.pri)
include(excel/excel.pri)
include(sqlcom/sqlcom.pri)
include(logcom/logcom.pri)
include(cfgcom/cfgcom.pri)
include(msgcom/msgcom.pri)
include(tabcom/tabcom.pri)
include(printer/printer.pri)
include(network/network.pri)
include(websocket/websocket.pri)
lessThan(QT_MAJOR_VERSION, 6) {
include(smtpclient/smtpclient.pri)
include(snmpclient/snmpclient.pri)
}
include(serialport/serialport.pri)
include(backcolour/backcolour.pri)
include(qtsingleapplication/qtsingleapplication.pri)

HEADERS += \
    $$PWD/print.h
	
SOURCES += \
    $$PWD/print.cpp

FORMS += \

RESOURCES += \
    $$PWD/images/image.qrc




