QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_iotvp_identification.cpp \
            ../../lib/Protocols/iotvp_identification.cpp \
#            ../../lib/Protocols/iotvp_read_write.cpp \
#            ../../lib/Protocols/iotvp_abstractreadwrite.cpp \
            ../../lib/Protocols/iotvp_abstractbody.cpp \
#            ../../lib/raw/raw.cpp

INCLUDEPATH += ../../lib/Protocols \
            ../../lib/raw
