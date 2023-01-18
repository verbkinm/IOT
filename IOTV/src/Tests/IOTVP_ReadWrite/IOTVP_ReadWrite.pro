QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_iotvp_readwrite.cpp \
            ../../lib/Protocols/iotvp_read_write.cpp \
            ../../lib/Protocols/iotvp_abstractreadwrite.cpp \
            ../../lib/Protocols/iotvp_abstractbody.cpp

#HEADERS +=  ../../lib/Protocols/iotvp_read_write.h \
#            ../../lib/Protocols/iotvp_abstractreadwrite.h \
#            ../../lib/Protocols/iotvp_abstractbody.h

INCLUDEPATH += ../../lib/Protocols \
#            ../../lib/raw
