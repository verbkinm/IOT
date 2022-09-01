QT += quick

CONFIG += c++2a

SOURCES += \
        client.cpp \
        device.cpp \
        main.cpp

resources.prefix = /$${TARGET}
RESOURCES += \
    qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    client.h \
    device.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/lib/Base_Host/ -lBase_Host
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/lib/Base_Host/ -lBase_Hostd
else:unix:!macx: LIBS += -L$$OUT_PWD/../src/lib/Base_Host/ -lBase_Host

INCLUDEPATH += $$PWD/../src/lib/Base_Host
DEPENDPATH += $$PWD/../src/lib/Base_Host

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/lib/Protocols/ -lProtocols
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/lib/Protocols/ -lProtocolsd
else:unix:!macx: LIBS += -L$$OUT_PWD/../src/lib/Protocols/ -lProtocols

INCLUDEPATH += $$PWD/../src/lib/Protocols
DEPENDPATH += $$PWD/../src/lib/Protocols

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/lib/Log/ -lLog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/lib/Log/ -lLogd
else:unix:!macx: LIBS += -L$$OUT_PWD/../src/lib/Log/ -lLog

INCLUDEPATH += $$PWD/../src/lib/Log
DEPENDPATH += $$PWD/../src/lib/Log

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/lib/raw/release/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/lib/raw/debug/ -lraw
else:unix:!macx: LIBS += -L$$OUT_PWD/../src/lib/raw/ -lraw

INCLUDEPATH += $$PWD/../src/lib/raw
DEPENDPATH += $$PWD/../src/lib/raw
