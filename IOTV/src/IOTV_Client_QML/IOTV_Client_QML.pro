QT += quick
#multimedia

CONFIG += c++2a

SOURCES += \
        client.cpp \
        device.cpp \
        ../lib/Base_Host/base_host.cpp \
        ../lib/Base_Host/channel.cpp \
        ../lib/Log/log.cpp \
        ../lib/Protocols/IOTV_SC.cpp \
        ../lib/Protocols/IOTV_SH.cpp \
        ../lib/Protocols/iotvp_header.cpp \
        ../lib/Protocols/iotvp_body.cpp \
        ../lib/raw/raw.cpp \
        ../lib/Protocols/iotvp_read_write.cpp \
        ../lib/Protocols/iotvp_state.cpp \
        ../lib/Protocols/iotvp_identification.cpp \
        main.cpp

resources.prefix = /$${TARGET}
RESOURCES += \
    qml.qrc

INCLUDEPATH += ../lib/Base_Host \
            ../lib/Log \
            ../lib/Protocols \
            ../lib/raw

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
    device.h \
    ../lib/Base_Host/base_host.h \
    ../lib/Base_Host/channel.h \
    ../lib/Log/log.h \
    ../lib/Protocols/IOTV_SC.h \
    ../lib/Protocols/IOTV_SH.h \
    ../lib/Protocols/iotvp_header.h \
    ../lib/Protocols/iotvp_body.h \
    ../lib/Protocols/protocols.h \
    ../lib/raw/raw.h \
    ../lib/Protocols/iotvp_read_write.h \
    ../lib/Protocols/iotvp_state.h \
    ../lib/Protocols/iotvp_identification.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,x86) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
