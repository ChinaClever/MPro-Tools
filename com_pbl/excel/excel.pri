INCLUDEPATH += $$PWD

win32: {
    QT += axcontainer
#    LIBS += -L$$PWD/lib/ -licom
#    CONFIG += qaxcontainer
} else: {
}

# 默认用 QXlsx 写 xlsx，不依赖本机安装 Excel。
# 改回 Windows OLE/QExcel：注释掉下一行后重新 qmake。
DEFINES += EXCEL_USE_QXLSX

QXLSX_PARENTPATH = $$PWD/QXlsx/
QXLSX_HEADERPATH = $$PWD/QXlsx/header/
QXLSX_SOURCEPATH = $$PWD/QXlsx/source/
include($$PWD/QXlsx/QXlsx.pri)

DEPENDPATH += $$PWD


HEADERS += \
    $$PWD/qexcel.h \
    $$PWD/excel_savethread.h

SOURCES += \
    $$PWD/qexcel.cpp \
    $$PWD/excel_savethread.cpp

