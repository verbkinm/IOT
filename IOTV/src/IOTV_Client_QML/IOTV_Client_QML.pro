QT += quick
#multimedia

CONFIG += c++2a

SOURCES += \
        client.cpp \
        device.cpp \
        lib/Base_Host/base_host.cpp \
        lib/Base_Host/channel.cpp \
        lib/Log/log.cpp \
        lib/Protocols/IOTV_SC.cpp \
        lib/Protocols/IOTV_SH.cpp \
        lib/raw/raw.cpp \
        main.cpp

resources.prefix = /$${TARGET}
RESOURCES += \
    qml.qrc

INCLUDEPATH += lib/Base_Host \
            lib/Log \
            lib/Protocols \
            lib/raw

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    client.h \
    device.h \
    lib/Base_Host/base_host.h \
    lib/Base_Host/channel.h \
    lib/Log/log.h \
    lib/Protocols/IOTV_SC.h \
    lib/Protocols/IOTV_SH.h \
    lib/Protocols/protocols.h \
    lib/raw/raw.h

SUBDIRS += \
    lib/Base_Host/Base_Host.pro
