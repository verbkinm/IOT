QT -= gui
QT += network

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Protocols/IOTV_SH.cpp \
        Protocols/embedded/creatorpkgs.cpp \
        Protocols/embedded/header.cpp \
        Protocols/embedded/identification.cpp \
        Protocols/embedded/read_write.cpp \
        Protocols/embedded/state.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Protocols/IOTV_SH.h \
    Protocols/embedded/creatorpkgs.h \
    Protocols/embedded/header.h \
    Protocols/embedded/identification.h \
    Protocols/embedded/read_write.h \
    Protocols/embedded/state.h \
    iot_server.h
