QT -= gui
QT += network

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Protocols/IOTV_SH.cpp \
        iot_server.cpp \
        main.cpp \
        network_module.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Protocols/IOTV_SH.h \
    iot_server.h \
    network_module.h


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
