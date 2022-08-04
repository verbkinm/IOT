QT -= gui
QT += network serialport testlib

CONFIG += c++2a console warn_on depend_includepath testcase
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        connection_type/base_conn_type.cpp \
        connection_type/com_conn_type.cpp \
        IOT_Server/iot_server.cpp \
        connection_type/file_conn_type.cpp \
        connection_type/tcp_conn_type.cpp \
        main.cpp \
        IOT_Host/IOT_Host.cpp \
        connection_type/udp_conn_type.cpp \
#tst_server.cpp \
        wrapper.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    IOT_Host/iot_host_structsettings.h \
    connection_type/base_conn_type.h \
    connection_type/com_conn_type.h \
    IOT_Host/IOT_Host.h \
    IOT_Server/iot_server.h \
    connection_type/file_conn_type.h \
    connection_type/tcp_conn_type.h \
    connection_type/udp_conn_type.h \
    wrapper.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:unix: LIBS += -L$$OUT_PWD/../lib/raw/ -lraw

INCLUDEPATH += $$PWD/../lib/raw
DEPENDPATH += $$PWD/../lib/raw

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -llog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -llog
else:unix: LIBS += -L$$OUT_PWD/../lib/Log/ -lLog

INCLUDEPATH += $$PWD/../lib/Log
DEPENDPATH += $$PWD/../lib/Log

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host
else:unix: LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lBase_Host

INCLUDEPATH += $$PWD/../lib/Base_Host
DEPENDPATH += $$PWD/../lib/Base_Host

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
else:unix: LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols

INCLUDEPATH += $$PWD/../lib/Protocols
DEPENDPATH += $$PWD/../lib/Protocols
