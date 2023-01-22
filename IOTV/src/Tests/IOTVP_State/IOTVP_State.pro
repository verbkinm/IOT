QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_iotvp_state_test.cpp \
            ../../lib/Protocols/iotvp_state.cpp \
            ../../lib/Protocols/iotvp_abstractbody.cpp \
            ../../lib/Protocols/iotvp_abstractreadwrite.cpp \
            ../../lib/Protocols/iotvp_abstract.cpp \
            ../../lib/Protocols/iotvp_creator.cpp \
            ../../lib/Protocols/iotvp_read_write.cpp \
            ../../lib/Protocols/iotvp_header.cpp \
            ../../lib/Protocols/iotvp_identification.cpp \
            ../../lib/raw/raw.cpp

#HEADERS +=  ../../lib/Protocols/iotvp_state.h \
#            ../../lib/Protocols/iotvp_abstractbody.h \
#            ../../lib/Protocols/iotvp_abstractreadwrite.h \
#            ../../lib/Protocols/iotvp_read_write.h \
#            ../../lib/raw/raw.h

INCLUDEPATH +=  ../../lib/Protocols \
                ../../lib/raw
