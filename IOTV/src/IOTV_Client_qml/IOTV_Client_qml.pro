QT += quick network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        device.cpp \
        lib/Base_Host/base_host.cpp \
        lib/Base_Host/channel.cpp \
        lib/Base_Host/read_channel.cpp \
        lib/Base_Host/write_channel.cpp \
        lib/Log/log.cpp \
        lib/Protocols/IOTV_SH.cpp \
        lib/Protocols/iotv_sc.cpp \
        lib/raw/raw.cpp \
        main.cpp \
        server.cpp \
        wrapper_device.cpp \
        wrapper_raw.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Patterns/Subject.h \
    Patterns/observer.h \
    device.h \
    lib/Base_Host/Base_Host_global.h \
    lib/Base_Host/base_host.h \
    lib/Base_Host/channel.h \
    lib/Base_Host/read_channel.h \
    lib/Base_Host/write_channel.h \
    lib/Log/Log_global.h \
    lib/Log/log.h \
    lib/Protocols/IOTV_SH.h \
    lib/Protocols/Protocols_global.h \
    lib/Protocols/iotv_sc.h \
    lib/Protocols/protocols.h \
    lib/raw/raw.h \
    server.h \
    wrapper_device.h \
    wrapper_raw.h

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
#else:unix: LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
#else:unix: LIBS += -L$$OUT_PWD/../lib/raw/ -lraw_x86
#else:unix: LIBS += -L. -lraw_x86

#INCLUDEPATH += $$PWD/../lib/raw
#DEPENDPATH += $$PWD/../lib/raw

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -lLog
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -lLog
##else:unix: LIBS += -L$$OUT_PWD/../lib/Log/ -lLog
#else:unix: LIBS += -L$$OUT_PWD/../lib/Log/ -lLog_x86

#INCLUDEPATH += $$PWD/../lib/Log
#DEPENDPATH += $$PWD/../lib/Log

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host
##else:unix: LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lBase_Host
#else:unix: LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lBase_Host_x86

#INCLUDEPATH += $$PWD/../lib/Base_Host
#DEPENDPATH += $$PWD/../lib/Base_Host


#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
##else:unix: LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
#else:unix: LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols_x86

#INCLUDEPATH += $$PWD/../lib/Protocols
#DEPENDPATH += $$PWD/../lib/Protocols

#SUBDIRS += \
#    lib/Base_Host/Base_Host.pro \
#    lib/Log/Log.pro \
#    lib/Protocols/Protocols.pro \
#    lib/lib.pro \
#    lib/raw/raw.pro
