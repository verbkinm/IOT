import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import "qrc:/Devices/" as Devices

Page {
    //Ссылка на Device
    required property var device
    property int switchStream: 0

    id: root
    title: device.aliasName
    objectName: device.aliasName

    header: Devices.DeviceHeader {
        id: headerPanel
    }

    Flickable {
        id: fl
        width: root.width
        height: root.height
        enabled: device.state

//        contentHeight: img.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Loader {
            id: imgLoader
//            source: "Image_cam.qml"
        }
    }

    Component.onCompleted: {
        console.log("Device 8 construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Device 8 destruct: ", objectName)
    }

    Devices.BusyRect {
        id: busyRect
        anchors.fill: parent
        visible: !device.state
    }

    Timer {
        id: t1
        interval: 1000;
        running: true;
//        repeat: true
        onTriggered: {
            imgLoader.source = ""
            t1.stop()
            t2.start()
        }
    }

    Timer {
        id: t2
        interval: 1000;
        running: false;
//        repeat: true
        onTriggered: {
            imgLoader.source = "Image_cam.qml"
            t2.stop()
            t1.start()
        }
    }
}

