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

        contentHeight: img.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Image {
            id: img
            source: "qrc:/img/logo.png"
            width: 200
            height: 200

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    switchStream++
                    if (switchStream % 2 == 0)
                    {
                        console.log("close stream")
                        device.signalCloseReadStream(0)
                    }
                    else
                    {
                        console.log("open stream")
                        device.signalOpenReadStream(0)
                    }
                }
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
}

