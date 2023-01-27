QT -= gui
QT += network serialport testlib

CONFIG += c++2a console warn_on depend_includepath testcase
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../lib/Protocols/embedded/IOTV_SH.cpp \
    ../lib/Protocols/embedded/creatorpkgs.cpp \
    ../lib/Protocols/embedded/header.cpp \
    ../lib/Protocols/embedded/identification.cpp \
    ../lib/Protocols/embedded/iotv_server_embedded.cpp \
    ../lib/Protocols/embedded/read_write.cpp \
    ../lib/Protocols/embedded/state.cpp \
    ConfigTypes.cpp \
    IOTV_Server/iotv_server.cpp \
    IOTV_Host/iotv_host.cpp \
    IOTV_Client/iotv_client.cpp \
    \
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
        ../lib/raw/raw.cpp \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../lib/Protocols/embedded/IOTV_SH.h \
    ../lib/Protocols/embedded/creatorpkgs.h \
    ../lib/Protocols/embedded/header.h \
    ../lib/Protocols/embedded/identification.h \
    ../lib/Protocols/embedded/iotv_server_embedded.h \
    ../lib/Protocols/embedded/read_write.h \
    ../lib/Protocols/embedded/state.h \
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
    ../lib/Protocols/protocols.h \
    ../lib/raw/raw.h

INCLUDEPATH += ../lib/Base_Host \
            ../lib/Log \
            ../lib/Protocols/embedded \
            ../lib/Protocols/ \
            ../lib/raw
