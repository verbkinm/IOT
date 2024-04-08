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

    Flickable {
        id: fl
        width: root.width
        height: root.height
        enabled: device.state

        contentHeight: camRect.height + 130 + 15

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        MyCamRect {
            id: camRect
            device: root.device
            width: root.width - 20
            height: root.height - root.height * 0.3
            anchors.horizontalCenter: parent.horizontalCenter

            onPlay: {
                device.signalOpenReadStream(0)
                device.signalOpenReadStream(1)
            }

            onStop: {
                device.signalCloseReadStream(0)
                device.signalCloseReadStream(1)
            }

            //            onVolumeOn: {
            //                device.signalOpenReadStream(1)
            //            }

            //            onVolumeOff: {
            //                device.signalCloseReadStream(1)
            //            }
        }

        onHeightChanged: {
//            console.log(fl.height, fl.contentHeight)
            glob_swipeView.focus = true // для glob_swipeView Keys.onEscapePressed:
        }
    }

    Component.onCompleted: {
        console.log("Device 8 construct: ", objectName)
//        console.log(root.height)
//        fl.contentHeight = camRect.height + 80 + 15
    }

    Component.onDestruction: {
        console.log("Device 8 destruct: ", objectName)
    }

    Devices.BusyRect {
        id: busyRect
        anchors.fill: parent
        visible: !device.state
    }
}

