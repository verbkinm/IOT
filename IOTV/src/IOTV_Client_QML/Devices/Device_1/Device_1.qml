import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/" as Devices

Page {
    //Ссылка на Device
    required property var device

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

        contentHeight: column.height

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
                model: [0, 1, 2]//device.readChannelLength

                //!!!
                Device_1_switch {
                    device: root.device
                    channel: Number(model.index)
                }
            }
        }

    }

    Component.onCompleted: {
        console.log("Device 1 construct: ", objectName)
        if (device.readChannelLength < device.writeChannelLength)
        {
            column.destroy()
            loaderMainItem.setSource("qrc:/DialogShared.qml", {parent: appStack,
                                             visible: true,
                                             title: "Ошибка устройства",
                                             text: "каналов чтения меньше чем каналов записи",
                                             standardButtons: Dialog.Ok})
            busyRect.visible = true
        }
        if (device.readChannelLength === 0)
        {
            column.destroy()
            loaderMainItem.setSource("qrc:/DialogShared.qml", {parent: appStack,
                                             visible: true,
                                             title: "Ошибка устройства",
                                             text: "каналы чтения отсутствуют",
                                             standardButtons: Dialog.Ok})
            busyRect.visible = true
        }
    }

    Component.onDestruction: {
        console.log("Device 1 destruct: ", objectName)
    }

    Devices.BusyRect {
        id: busyRect
        anchors.fill: parent
        visible: !device.state
    }

}

