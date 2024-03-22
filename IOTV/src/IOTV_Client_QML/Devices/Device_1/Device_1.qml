import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/" as Devices

Page {
    id: root
    title: device.aliasName
    objectName: device.aliasName

    //Ссылка на Device
    required property var device

    //    property bool firstRX: false
    header: Devices.DeviceHeader {
        id: headerPanel
    }

    Flickable {
        id: fl
        width: root.width
        height: root.height
        enabled: device.state

        contentHeight: column.height + column.topPadding + column.spacing + overlayHeader.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            spacing: 15
            topPadding: 15

            Repeater {
                id: repeater
                model: device.readChannelLength

                Device_1_switch {
                    required property int index
                    device: root.device
                    channel: index
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Device 1 construct: ", objectName)
        if (device.readChannelLength < device.writeChannelLength) {
            column.destroy()

            glob_dialogShared.defaultAcceptedMessage()
            glob_dialogShared.title = "Ошибка устройства"
            glob_dialogShared.text = "каналов чтения меньше чем каналов записи"
            glob_dialogShared.open()

            busyRect.visible = true
        }
        if (device.readChannelLength === 0) {
            column.destroy()

            glob_dialogShared.defaultAcceptedMessage()
            glob_dialogShared.title = "Ошибка устройства"
            glob_dialogShared.text = "каналы чтения отсутствуют"
            glob_dialogShared.open()

            busyRect.visible = true
        }
    }

    Component.onDestruction: {
        console.log("Device 1 destruct: ", objectName)
    }

    Devices.BusyRect {
        id: busyRect
        anchors.fill: parent
        visible: !device.state /*|| (firstRX === false)*/
    }
}
