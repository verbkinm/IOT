QT -= gui

TEMPLATE = lib
DEFINES += PROTOCOLS_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    IOTV_SH.cpp \
#    iotv_sc.cpp

HEADERS += \
    IOTV_SH.h \
    Protocols_global.h \
    iotv_sc.h \
    protocols.h

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

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Log/ -llog
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Log/ -llog
##else:unix: LIBS += -L$$OUT_PWD/../Log/ -llog
#else:unix: LIBS += -L$$OUT_PWD/../Log/ -lLog

#INCLUDEPATH += $$PWD/../Log
#DEPENDPATH += $$PWD/../Log

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Base_Host/ -lbase_host
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Base_Host/ -lbase_host
#else:unix: LIBS += -L$$OUT_PWD/../Base_Host/ -lBase_Host

#INCLUDEPATH += $$PWD/../Base_Host
#DEPENDPATH += $$PWD/../Base_Host
