
INCLUDEPATH += $$PWD

# ssdp_obj.cpp 直接调用 setsockopt，需要链接 Winsock。
win32: LIBS += -lws2_32



HEADERS += \
    $$PWD/ssdp_core.h \
    $$PWD/ssdp_obj.h \
    $$PWD/ssdp_search.h \
    $$PWD/ssdp_setting.h
	
SOURCES += \
    $$PWD/ssdp_core.cpp \
    $$PWD/ssdp_obj.cpp \
    $$PWD/ssdp_search.cpp \
    $$PWD/ssdp_setting.cpp




