QT += quick network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        client.cpp \
#        device.cpp \
        main.cpp \
#        wrapper_device.cpp \
#        wrapper_raw.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
#QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Patterns/Subject.h \
    Patterns/observer.h \
    client.h \
#    device.h \
#    wrapper_device.h \
#    wrapper_raw.h


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lBase_Host
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lBase_Hostd
else:unix:!macx: LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lBase_Host

INCLUDEPATH += $$PWD/../lib/Base_Host
DEPENDPATH += $$PWD/../lib/Base_Host

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -lLog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -lLogd
else:unix:!macx: LIBS += -L$$OUT_PWD/../lib/Log/ -lLog

INCLUDEPATH += $$PWD/../lib/Log
DEPENDPATH += $$PWD/../lib/Log

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocolsd
else:unix:!macx: LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols

INCLUDEPATH += $$PWD/../lib/Protocols
DEPENDPATH += $$PWD/../lib/Protocols

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lrawd
else:unix:!macx: LIBS += -L$$OUT_PWD/../lib/raw/ -lraw

INCLUDEPATH += $$PWD/../lib/raw
DEPENDPATH += $$PWD/../lib/raw

DISTFILES +=
