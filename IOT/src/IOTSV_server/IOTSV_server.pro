QT -= gui
QT += network

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        IOT_Host/shedule.cpp \
        channels/channel.cpp \
        channels/channelmanager.cpp \
        channels/read_channel.cpp \
        channels/write_channel.cpp \
        connection_type/base_conn_type.cpp \
        connection_type/com_conn_type.cpp \
        #connection_type/connection_type.cpp \
        connection_type/ethernet_conn_type.cpp \
        main.cpp \
        IOT_Host/IOT_Host.cpp \
        protocol_class.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    IOT_Host/shedule.h \
    channels/channel.h \
    channels/channelmanager.h \
    channels/read_channel.h \
    channels/write_channel.h \
    connection_type/base_conn_type.h \
    connection_type/com_conn_type.h \
    #connection_type/connection_type.h \
    connection_type/ethernet_conn_type.h \
    IOT_Host/IOT_Host.h \
    protocol_class.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:unix: LIBS += -L$$OUT_PWD/../lib/raw/ -lraw

INCLUDEPATH += $$PWD/../lib/raw
DEPENDPATH += $$PWD/../lib/raw

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -lLog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -lLog
else:unix: LIBS += -L$$OUT_PWD/../lib/Log/ -lLog

INCLUDEPATH += $$PWD/../lib/Log
DEPENDPATH += $$PWD/../lib/Log
