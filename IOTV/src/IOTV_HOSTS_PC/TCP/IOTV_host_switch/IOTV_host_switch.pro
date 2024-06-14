QT -= gui
QT += network

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../lib/Protocols/embedded/IOTV_SH.c \
        ../../../lib/Protocols/embedded/creatorpkgs.c \
        ../../../lib/Protocols/embedded/header.c \
        ../../../lib/Protocols/embedded/identification.c \
        ../../../lib/Protocols/embedded/iotv_server_embedded.c \
        ../../../lib/Protocols/embedded/read_write.c \
        ../../../lib/Protocols/embedded/state.c \
        ../../../lib/Protocols/embedded/tech.c \
        ../../../lib/Protocols/embedded/host_broadcast.c  \
        ../../../lib/Protocols/embedded/log_data.c  \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../lib/Protocols/embedded/IOTV_SH.h \
    ../../../lib/Protocols/embedded/creatorpkgs.h \
    ../../../lib/Protocols/embedded/header.h \
    ../../../lib/Protocols/embedded/identification.h \
    ../../../lib/Protocols/embedded/iotv_server_embedded.h \
    ../../../lib/Protocols/embedded/read_write.h \
    ../../../lib/Protocols/embedded/state.h \
    ../../../lib/Protocols/embedded/tech.h \
    ../../../lib/Protocols/embedded/host_broadcast.h  \
    ../../../lib/Protocols/embedded/log_data.h

INCLUDEPATH += ../../../lib/Protocols/embedded/
