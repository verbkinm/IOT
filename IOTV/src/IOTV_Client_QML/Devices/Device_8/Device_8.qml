import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem

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

//    onVisibleChanged: {
//        if (!visible)
//            device.signalCloseReadStream(0)
//    }

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

        MyCamRect {
            id: camRect
            width: parent.width - 20
            height: parent.height - (parent.height / 100 * 30)
            anchors.horizontalCenter: parent.horizontalCenter

            onPlay: {
                device.signalOpenReadStream(0)
            }

            onStop: {
                device.signalCloseReadStream(0)
            }
        }
    }

    Connections {
        target: device
        function onSignalDataPkgComplete(channel) {
            if (channel === 0)
            {
                camRect.reloadImage()
                console.log("onSignalDataPkgComplete")
            }
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

    //    Timer {
    //        id: t1
    //        interval: 500;
    //        running: true;
    //        repeat: true
    //        onTriggered: {
    //            img.reloadImage()
    //        }
    //    }
}

