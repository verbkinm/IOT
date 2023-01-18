QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_iotvp_header_test.cpp \
            ../../lib/Protocols/iotvp_header.cpp \
            ../../lib/Protocols/iotvp_identification.cpp \
            ../../lib/Protocols/iotvp_abstractbody.cpp \
            ../../lib/Protocols/iotvp_read_write.cpp \
            ../../lib/Protocols/iotvp_abstractreadwrite.cpp \
            ../../lib/Protocols/iotvp_state.cpp \
            ../../lib/raw/raw.cpp

HEADERS += ../../lib/raw/raw.h

INCLUDEPATH +=  ../../lib/Protocols \
                ../../lib/raw


