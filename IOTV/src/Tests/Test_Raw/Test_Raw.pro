QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_test_raw.cpp \
            ../../lib/raw/raw.cpp


INCLUDEPATH += $$PWD/../../lib/raw
DEPENDPATH += $$PWD/../../lib/raw
