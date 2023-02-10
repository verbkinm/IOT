QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../src/lib/Protocols/embedded/IOTV_SH.cpp \
    ../../src/lib/Protocols/embedded/creatorpkgs.cpp \
    ../../src/lib/Protocols/embedded/header.cpp \
    ../../src/lib/Protocols/embedded/identification.cpp \
    ../../src/lib/Protocols/embedded/iotv_server_embedded.cpp \
    ../../src/lib/Protocols/embedded/iotvp_print.cpp \
    ../../src/lib/Protocols/embedded/read_write.cpp \
    ../../src/lib/Protocols/embedded/state.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../../src/lib/Protocols/embedded/IOTV_SH.h \
    ../../src/lib/Protocols/embedded/creatorpkgs.h \
    ../../src/lib/Protocols/embedded/header.h \
    ../../src/lib/Protocols/embedded/identification.h \
    ../../src/lib/Protocols/embedded/iotv_server_embedded.h \
    ../../src/lib/Protocols/embedded/iotvp_print.h \
    ../../src/lib/Protocols/embedded/read_write.h \
    ../../src/lib/Protocols/embedded/state.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += "../../src/lib/Protocols/embedded/"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
