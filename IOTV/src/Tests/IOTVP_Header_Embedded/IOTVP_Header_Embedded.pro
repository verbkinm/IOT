QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_iotvp_header_embedded_test.cpp \
            ../../lib/Protocols/embedded/creatorpkgs.c \
            ../../lib/Protocols/embedded/header.c \
            ../../lib/Protocols/embedded/identification.c \
            ../../lib/Protocols/embedded/read_write.c \
            ../../lib/Protocols/embedded/state.c

HEADERS +=  ../../lib/Protocols/embedded/creatorpkgs.h \
            ../../lib/Protocols/embedded/header.h \
            ../../lib/Protocols/embedded/identification.h \
            ../../lib/Protocols/embedded/read_write.h \
            ../../lib/Protocols/embedded/state.h

INCLUDEPATH += INCLUDEPATH +=  ../../lib/Protocols/embedded

