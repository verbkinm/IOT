QT -= gui

TEMPLATE = lib
DEFINES += BASE_HOST_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    base_host.cpp \
    channel.cpp \
    read_channel.cpp \
    write_channel.cpp

HEADERS += \
    Base_Host_global.h \
    base_host.h \
    channel.h \
    read_channel.h \
    write_channel.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../raw/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../raw/ -lraw
else:unix: LIBS += -L$$OUT_PWD/../raw/ -lraw

INCLUDEPATH += $$PWD/../raw
DEPENDPATH += $$PWD/../raw

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Log/ -llog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Log/ -llog
else:unix: LIBS += -L$$OUT_PWD/../Log/ -lLog

INCLUDEPATH += $$PWD/../Log
DEPENDPATH += $$PWD/../Log
