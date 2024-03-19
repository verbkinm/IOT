#QT -= gui
QT += gui network multimedia widgets

CONFIG += c++17
CONFIG -= app_bundle
#LIBS += -lssl -lcrypto

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../lib/Protocols/embedded/IOTV_SH.cpp \
        ../../lib/Protocols/embedded/creatorpkgs.cpp \
        ../../lib/Protocols/embedded/header.cpp \
        ../../lib/Protocols/embedded/identification.cpp \
        ../../lib/Protocols/embedded/iotv_server_embedded.cpp \
        ../../lib/Protocols/embedded/read_write.cpp \
        ../../lib/Protocols/embedded/state.cpp \
        ../../lib/Protocols/embedded/tech.cpp \
        ../../lib/Protocols/embedded/host_broadcast.cpp  \
        main.cpp \
        widget.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../lib/Protocols/embedded/IOTV_SH.h \
    ../../lib/Protocols/embedded/creatorpkgs.h \
    ../../lib/Protocols/embedded/header.h \
    ../../lib/Protocols/embedded/identification.h \
    ../../lib/Protocols/embedded/iotv_server_embedded.h \
    ../../lib/Protocols/embedded/read_write.h \
    ../../lib/Protocols/embedded/state.h \
    ../../lib/Protocols/embedded/tech.h \
    ../../lib/Protocols/embedded/host_broadcast.h  \
#    iot_server.h \
    widget.h

INCLUDEPATH += ../../lib/Protocols/embedded/
