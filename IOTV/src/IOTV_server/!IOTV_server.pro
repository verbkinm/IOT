QT -= gui
QT += network serialport
#testlib

CONFIG += c++2a console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += debug_and_release

#CONFIG(debug,debug|release) DESTDIR = debug
#CONFIG(release,debug|release) DESTDIR = release

TARGET = iotv_server

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../lib/ConfigType/ConfigTypes.cpp \
    ../lib/Events/actions/iotv_action.cpp \
    ../lib/Events/actions/iotv_action_data_tx.cpp \
    ../lib/Events/actions/iotv_action_data_tx_ref.cpp \
    ../lib/Events/actions/iotv_action_msg.cpp \
    ../lib/Events/events/iotv_event.cpp \
    ../lib/Events/events/iotv_event_connect.cpp \
    ../lib/Events/events/iotv_event_data.cpp \
    ../lib/Events/events/iotv_event_disconnect.cpp \
    ../lib/Events/events/iotv_event_state.cpp \
    ../lib/Events/events/iotv_event_alarm.cpp \
    ../lib/Events/events/iotv_event_timer.cpp \
    ../lib/Events/iotv_event_manager.cpp \
    ../lib/Json_Parsers/event_action_parser.cpp \
    ../lib/Protocols/embedded/IOTV_SH.c \
    ../lib/Protocols/embedded/creatorpkgs.c \
    ../lib/Protocols/embedded/header.c \
    ../lib/Protocols/embedded/identification.c \
    ../lib/Protocols/embedded/iotv_server_embedded.c \
    ../lib/Protocols/embedded/read_write.c \
    ../lib/Protocols/embedded/state.c \
    ../lib/Protocols/embedded/tech.c \
    ../lib/Protocols/embedded/host_broadcast.c  \
    ../lib/Protocols/embedded/log_data.c  \
    ../lib/ThreadPool/thread_pool.cpp \
    ../lib/ThreadPool/thread_safe_queue.cpp \
    IOTV_Server/iotv_server.cpp \
    IOTV_Host/iotv_host.cpp \
    IOTV_Client/iotv_client.cpp \
    IOTV_Server/maker_iotv.cpp \
    connection_type/base_conn_type.cpp \
    connection_type/com_conn_type.cpp \
    connection_type/file_conn_type.cpp \
    connection_type/tcp_conn_type.cpp \
     connection_type/tcp_reverse_conn_type.cpp \
    main.cpp \
    connection_type/udp_conn_type.cpp \
    wrapper.cpp \
    ../lib/Base_Host/base_host.cpp \
    ../lib/Base_Host/channel.cpp \
    ../lib/Log/log.cpp \
    ../lib/raw/raw.cpp \
    ../lib/raii/raii_header.cpp \
    ../lib/raii/raii_iot.cpp \

HEADERS += \
    ../lib/ConfigType/ConfigTypes.h \
    ../lib/Events/actions/iotv_action.h \
    ../lib/Events/actions/iotv_action_data_tx.h \
    ../lib/Events/actions/iotv_action_data_tx_ref.h \
    ../lib/Events/actions/iotv_action_msg.h \
    ../lib/Events/events/iotv_event.h \
    ../lib/Events/events/iotv_event_connect.h \
    ../lib/Events/events/iotv_event_data.h \
    ../lib/Events/events/iotv_event_disconnect.h \
    ../lib/Events/events/iotv_event_state.h \
    ../lib/Events/events/iotv_event_alarm.h \
    ../lib/Events/events/iotv_event_timer.h \
    ../lib/Events/iotv_event_manager.h \
    ../lib/Json_Parsers/event_action_parser.h \
    ../lib/Protocols/embedded/IOTV_SH.h \
    ../lib/Protocols/embedded/creatorpkgs.h \
    ../lib/Protocols/embedded/header.h \
    ../lib/Protocols/embedded/identification.h \
    ../lib/Protocols/embedded/iotv_server_embedded.h \
    ../lib/Protocols/embedded/read_write.h \
    ../lib/Protocols/embedded/state.h \
#    ../lib/Protocols/embedded/iotvp_print.h \
    ../lib/Protocols/embedded/tech.h \
    ../lib/Protocols/embedded/host_broadcast.h \
    ../lib/Protocols/embedded/log_data.h  \
    ../lib/ThreadPool/thread_pool.h \
    ../lib/ThreadPool/thread_safe_queue.h \
    IOTV_Server/iotv_server.h \
    IOTV_Host/iotv_host.h \
    IOTV_Client/iotv_client.h \
    IOTV_Server/maker_iotv.h \
    connection_type/base_conn_type.h \
    connection_type/com_conn_type.h \
    connection_type/file_conn_type.h \
    connection_type/tcp_conn_type.h \
    connection_type/tcp_reverse_conn_type.h \
    connection_type/udp_conn_type.h \
    wrapper.h \
    ../lib/Base_Host/base_host.h \
    ../lib/Base_Host/channel.h \
    ../lib/Log/log.h \
    ../lib/raw/raw.h \
    ../lib/raii/raii_header.h \
    ../lib/raii/raii_iot.h \

INCLUDEPATH += ../lib/Base_Host \
                ../lib/Log \
                ../lib/Protocols/embedded \
                ../lib/Protocols/ \
                ../lib/raw \
                ../lib/ConfigType \
                ../lib/Events \
                ../lib/Json_Parsers \
                ../lib/ThreadPool \
                ../lib/raii \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
