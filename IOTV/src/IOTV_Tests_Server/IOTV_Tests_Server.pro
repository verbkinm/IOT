QT += testlib network serialport
QT -= gui

CONFIG += c++17 qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_server.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:unix: LIBS += -L$$OUT_PWD/../lib/raw/ -lraw

INCLUDEPATH += $$PWD/../lib/raw
DEPENDPATH += $$PWD/../lib/raw

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -llog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -llog
else:unix: LIBS += -L$$OUT_PWD/../lib/Log/ -lLog

INCLUDEPATH += $$PWD/../lib/Log
DEPENDPATH += $$PWD/../lib/Log

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host
else:unix: LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lBase_Host

INCLUDEPATH += $$PWD/../lib/Base_Host
DEPENDPATH += $$PWD/../lib/Base_Host

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
else:unix: LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols

INCLUDEPATH += $$PWD/../lib/Protocols
DEPENDPATH += $$PWD/../lib/Protocols
