import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import "qrc:/Devices/" as Devices
import "qrc:/Devices/Device_1" as Devices_1
import "qrc:/Devices/Device_2" as Devices_2
import "qrc:/Devices/BaseItem" as BaseItem


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

            // Выключатель
            Devices_1.Device_1_switch {
                id: btn_id5
                device: root.device
                channel: 0
            }

            BaseItem.MeteoBlock {
                id: meteoBlock
                width: parent.width
                height: 300

                device: root.device
                channelTemperature: 11
                channelHumidity: 12
                channelPressure: 13
            }

        }
    }

    Component.onCompleted: {
        console.log("Device 5 construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Device 5 destruct: ", objectName)
    }

    Devices.BusyRect {
        id: busyRect
        anchors.fill: parent
        visible: !device.state
    }
}

