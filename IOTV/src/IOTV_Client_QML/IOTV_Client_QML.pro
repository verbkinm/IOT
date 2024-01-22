QT += quick multimedia

CONFIG += c++2a

SOURCES += \
        ../lib/ConfigType/ConfigTypes.cpp \
        ../lib/Events/actions/iotv_action.cpp \
        ../lib/Events/actions/iotv_action_data_tx.cpp \
        ../lib/Events/actions/iotv_action_data_tx_ref.cpp \
        ../lib/Events/actions/iotv_action_msg.cpp \
        ../lib/Events/events/iotv_event.cpp \
        ../lib/Events/events/iotv_event_connect.cpp \
        ../lib/Events/events/iotv_event_data.cpp \
        ../lib/Events/events/iotv_event_disconnect.cpp \
        ../lib/Events/events/iotv_event_state.cpp \
        ../lib/Events/events/iotv_event_alarm.cpp \
        ../lib/Events/events/iotv_event_timer.cpp \
        ../lib/Events/iotv_event_manager.cpp \
        ../lib/Json_Parsers/event_action_parser.cpp \
        ../lib/Protocols/embedded/IOTV_SH.cpp \
        ../lib/Protocols/embedded/creatorpkgs.cpp \
        ../lib/Protocols/embedded/header.cpp \
        ../lib/Protocols/embedded/identification.cpp \
        ../lib/Protocols/embedded/iotv_server_embedded.cpp \
        ../lib/Protocols/embedded/iotvp_print.cpp \
        ../lib/Protocols/embedded/read_write.cpp \
        ../lib/Protocols/embedded/state.cpp \
        ../lib/Protocols/embedded/tech.cpp \
        client.cpp \
        device.cpp \
        ../lib/Base_Host/base_host.cpp \
        ../lib/Base_Host/channel.cpp \
        ../lib/Log/log.cpp \
        ../lib/raw/raw.cpp \
        generalaudiooutput.cpp \
        main.cpp \
        producer.cpp \
        wrap_qbytearray.cpp

HEADERS += \
    ../lib/ConfigType/ConfigTypes.h \
    ../lib/Events/actions/iotv_action.h \
    ../lib/Events/actions/iotv_action_data_tx.h \
    ../lib/Events/actions/iotv_action_data_tx_ref.h \
    ../lib/Events/actions/iotv_action_msg.h \
    ../lib/Events/events/iotv_event.h \
    ../lib/Events/events/iotv_event_connect.h \
    ../lib/Events/events/iotv_event_data.h \
    ../lib/Events/events/iotv_event_disconnect.h \
    ../lib/Events/events/iotv_event_state.h \
    ../lib/Events/events/iotv_event_alarm.h \
    ../lib/Events/events/iotv_event_timer.h \
    ../lib/Events/iotv_event_manager.h \
    ../lib/Json_Parsers/event_action_parser.h \
    ../lib/Protocols/embedded/IOTV_SH.h \
    ../lib/Protocols/embedded/creatorpkgs.h \
    ../lib/Protocols/embedded/header.h \
    ../lib/Protocols/embedded/identification.h \
    ../lib/Protocols/embedded/iotv_server_embedded.h \
    ../lib/Protocols/embedded/iotvp_print.h \
    ../lib/Protocols/embedded/read_write.h \
    ../lib/Protocols/embedded/state.h \
    ../lib/Protocols/embedded/tech.h \
    client.h \
    device.h \
    ../lib/Base_Host/base_host.h \
    ../lib/Base_Host/channel.h \
    ../lib/Log/log.h \
    ../lib/raw/raw.h \
    generalaudiooutput.h \
    producer.h \
    wrap_qbytearray.h

resources.prefix = /$${TARGET}
RESOURCES += \
    qml.qrc

INCLUDEPATH += ../lib/Base_Host \
            ../lib/Log \
            ../lib/Protocols/embedded \
            ../lib/Protocols/ \
            ../lib/raw \
            ../lib/ConfigType \
            ../lib/Events \
            ../lib/Json_Parsers

# Additional import path used to resolve QML modules in Qt Creator's code model
#QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
#QML_DESIGNER_IMPORT_PATH =



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

contains(ANDROID_TARGET_ARCH,x86) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,x86_64) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
