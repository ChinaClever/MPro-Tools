
INCLUDEPATH += $$PWD

include(excel/excel.pri)
include(msgcom/msgcom.pri)
include(tabcom/tabcom.pri)
include(backcolour/backcolour.pri)
include(qtsingleapplication/qtsingleapplication.pri)

CM_PATH = ../../../cores/commons
include($$CM_PATH/cms/cms.pri)
include($$CM_PATH/cfg/cfg.pri)
include($$CM_PATH/crc/crc.pri)
include($$CM_PATH/rpc/rpc.pri)
include($$CM_PATH/dtls/dtls.pri)
include($$CM_PATH/ssdp/ssdp.pri)
include($$CM_PATH/cthread/cthread.pri)
include($$CM_PATH/network/network.pri)
unix: include($$CM_PATH/ipc/ipc.pri)

HEADERS += 
	
SOURCES += 

FORMS += \




