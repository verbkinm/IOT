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
        ../lib/Base_Host/base_host.cpp \
        ../lib/Base_Host/channel.cpp \
        ../lib/Log/log.cpp \
        ../lib/Protocols/IOTV_SC.cpp \
        ../lib/Protocols/IOTV_SH.cpp \
        ../lib/raw/raw.cpp \

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
    wrapper.h \
    ../lib/Base_Host/base_host.h \
    ../lib/Base_Host/channel.h \
    ../lib/Log/log.h \
    ../lib/Protocols/IOTV_SC.h \
    ../lib/Protocols/IOTV_SH.h \
    ../lib/Protocols/protocols.h \
    ../lib/raw/raw.h

INCLUDEPATH += ../lib/Base_Host \
            ../lib/Log \
            ../lib/Protocols \
            ../lib/raw
