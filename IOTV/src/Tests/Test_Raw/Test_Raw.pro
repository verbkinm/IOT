QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_test_raw.cpp \
            ../../lib/raw/raw.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/raw/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/raw/ -lrawd
else:unix:!macx: LIBS += -L$$OUT_PWD/../../lib/raw/ -lraw

INCLUDEPATH += $$PWD/../../lib/raw
DEPENDPATH += $$PWD/../../lib/raw
