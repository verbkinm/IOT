QT -= gui
QT += network serialport testlib

CONFIG += c++2a console warn_on depend_includepath testcase
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ConfigTypes.cpp \
    IOTV_Server/iotv_server.cpp \
    IOTV_Host/iotv_host.cpp \
    IOTV_Client/iotv_client.cpp \
        connection_type/base_conn_type.cpp \
        connection_type/com_conn_type.cpp \
        connection_type/file_conn_type.cpp \
        connection_type/tcp_conn_type.cpp \
        main.cpp \
        connection_type/udp_conn_type.cpp \
        wrapper.cpp \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ConfigTypes.h \
    IOTV_Server/iotv_server.h \
    IOTV_Host/iotv_host.h \
    IOTV_Client/iotv_client.h \
    connection_type/base_conn_type.h \
    connection_type/com_conn_type.h \
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
