QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GUI/Devices/gui_base_device.cpp \
    GUI/Devices/gui_device_creator.cpp \
    GUI/Devices/gui_device_switch.cpp \
    GUI/Devices/gui_device_unknow.cpp \
    GUI/Devices/gui_device_weather_station.cpp \
    GUI/Devices/gui_edit_base_device.cpp \
    GUI/tab_room.cpp \
    GUI/gui_edit_server.cpp \
    GUI/gui_server.cpp \
    GUI/tab.cpp \
    device.cpp \
    GUI/devicelist.cpp \
    main.cpp \
    mainwindow.cpp \
    GUI/objectlist.cpp \
    server.cpp

HEADERS += \
    GUI/Devices/gui_base_device.h \
    GUI/Devices/gui_device_creator.h \
    GUI/Devices/gui_device_switch.h \
    GUI/Devices/gui_device_unknow.h \
    GUI/Devices/gui_device_weather_station.h \
    GUI/Devices/gui_edit_base_device.h \
    GUI/tab_room.h \
    GUI/gui_edit_server.h \
    GUI/gui_server.h \
    GUI/tab.h \
    Patterns/Subject.h \
    device.h \
    GUI/devicelist.h \
    mainwindow.h \
    Patterns/observer.h \
    GUI/objectlist.h \
    server.h

FORMS += \
    GUI/Devices/gui_edit_base_device.ui \
    GUI/gui_edit_server.ui \
    GUI/tab.ui \
    gui_server.ui \
    mainwindow.ui \
    GUI/objectlist.ui

RESOURCES += \
    img.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/raw/ -lraw
else:unix: LIBS += -L$$OUT_PWD/../lib/raw/ -lraw

INCLUDEPATH += $$PWD/../lib/raw
DEPENDPATH += $$PWD/../lib/raw

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -lLog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Log/ -lLog
else:unix: LIBS += -L$$OUT_PWD/../lib/Log/ -lLog

INCLUDEPATH += $$PWD/../lib/Log
DEPENDPATH += $$PWD/../lib/Log

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host
else:unix: LIBS += -L$$OUT_PWD/../lib/Base_Host/ -lbase_host

INCLUDEPATH += $$PWD/../lib/Base_Host
DEPENDPATH += $$PWD/../lib/Base_Host


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols
else:unix: LIBS += -L$$OUT_PWD/../lib/Protocols/ -lProtocols

INCLUDEPATH += $$PWD/../lib/Protocols
DEPENDPATH += $$PWD/../lib/Protocols
