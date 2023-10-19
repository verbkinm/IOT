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

        Image {
            id: img
            anchors.horizontalCenter: parent.horizontalCenter
            cache: false
            source: "file:Image.jpg"
//            sourceSize.width: 200
//            sourceSize.height: 200

            function reloadImage(stringPath) {
                var oldSource = source
                source = ""
                source = oldSource
            }
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

    Connections {
        target: device
        function onSignalDataPkgComplete(channel, data) {
            if (channel === 0)
            {
                img.reloadImage()
                console.log("onSignalDataPkgComplete")
            }
        }
    }
    Component.onCompleted: {
        console.log("Device 8 construct: ", objectName)
        device.signalOpenReadStream(0)
    }

    Component.onDestruction: {
        console.log("Device 8 destruct: ", objectName)
        device.signalCloseReadStream(0)
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

