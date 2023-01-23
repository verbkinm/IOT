QT += testlib network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_iotvp_header_embedded_test.cpp \
            ../../lib/Protocols/embedded/creatorpkgs.cpp \
            ../../lib/Protocols/embedded/header.cpp \
            ../../lib/Protocols/embedded/identification.cpp \
            ../../lib/Protocols/embedded/read_write.cpp \
            ../../lib/Protocols/embedded/state.cpp

HEADERS +=  ../../lib/Protocols/embedded/creatorpkgs.h \
            ../../lib/Protocols/embedded/header.h \
            ../../lib/Protocols/embedded/identification.h \
            ../../lib/Protocols/embedded/read_write.h \
            ../../lib/Protocols/embedded/state.h \
            ../../lib/Protocols/embedded/IOTV_SH.h \
            ../../IOTV_HOSTS_PC/IOTV_host_switch/iot_server.h

INCLUDEPATH +=  ../../lib/Protocols/embedded \
                ../../IOTV_HOSTS_PC/IOTV_host_switch/


