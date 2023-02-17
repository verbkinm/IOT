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

        contentHeight: column.height + 50

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            spacing: 15
            topPadding: 15

            //Температура
            Device_2_Item {
                id: temperature
                source: "qrc:/img/id_1/temperature.png"
                text: "0.00 °C"
            }

            //Влажность
            Device_2_Item {
                id: humidity
                source: "qrc:/img/id_1/humidity.png"
                text: "0.00 %"
            }

            //Давление
            Device_2_Item {
                id: pressure
                source: "qrc:/img/id_1/pressure.png"
                text: "0.00 мм рт.ст."
                visible: device.readChannelLength === 3
            }
        }
    }

    Component.onCompleted: {
        console.log("Device 2 construct: ", title)
    }

    Component.onDestruction: {
        console.log("Device 2 destruct: ", title)
    }

    Connections {
        target: device
        function onSignalDataChanged(channel) {
            var t = device.readData(0)
            if (t > 0)
                t = "+" + t

            temperature.text = t.slice(0, t.length - 2) + " °C"

            var h = device.readData(1)
            humidity.text = h.slice(0, h.length - 2) + " %"

            if (device.readChannelLength === 3)
            {
                var p = device.readData(2)
                pressure.text = p.slice(0, p.length - 2) + " мм рт.ст."
            }
        }
    }

    Devices.BusyRect {
        visible: !device.state
    }
}

