import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import "qrc:/Devices/" as Devices
import "qrc:/Devices/Device_1" as Devices_1
import "qrc:/Devices/Device_2" as Devices_2


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
                channel: 0
                btn.checked: device.readData(0) === "true"
                btn.onClicked: {
                    device.setDataFromString(0, (btn.checked ? "true" : "false"))
                    btn.toggle()
                    wait()
                    timer.start()
                }

                SequentialAnimation {
                    id: anim
                    PropertyAnimation {
                        target: btn_id5.rec
                        property: "color"
                        from: Qt.rgba(0, 0, 0, 0)
                        to: Qt.rgba(0, 0, 255, 0.5)
                        duration: 200
                    }
                    PropertyAnimation  {
                        target: btn_id5.rec
                        property: "color"
                        from: Qt.rgba(0, 0, 255, 0.5)
                        to: Qt.rgba(0, 0, 0, 0)
                        duration: 200
                    }
                }

                Timer {
                    id: timer
                    interval: 5000
                    repeat: false
                    onTriggered: {
                        btn_id5.notWait()
                        loaderMainItem.setSource("qrc:/Notification.qml", {parent: appStack, text: btn_id5.desciptionLabel.text + "\nответ не получен"})
                    }
                }
            }

            //Температура
            Devices_2.Device_2_Item {
                id: temperature
                source: "qrc:/img/id_1/temperature.png"
                text: "0.00 °C"
            }

            //Влажность
            Devices_2.Device_2_Item {
                id: humidity
                source: "qrc:/img/id_1/humidity.png"
                text: "0.00 %"
            }

            //Давление
            Devices_2.Device_2_Item {
                id: pressure
                source: "qrc:/img/id_1/pressure.png"
                text: "0.00 "//"мм рт.ст."
            }
        }
    }

    Component.onCompleted: {
        console.log("Device 5 construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Device 5 destruct: ", objectName)
    }

    Connections {
        target: device
        function onSignalDataChanged(channel) {
            if (channel === 0)
            {
                btn_id5.btn.checked = device.readData(0) === "true"
                anim.start()
                timer.stop()
            }
            else if (channel === 11)
            {
                var t = device.readData(11)
                if (t > 0)
                    t = "+" + t

                temperature.text = t.slice(0, t.length - 2) + " °C"
            }
            else if (channel === 12)
            {
                var h = device.readData(12)
                humidity.text = h.slice(0, h.length - 2) + " %"
            }
            else if (channel === 13)
            {
                var p = device.readData(13)
                pressure.text = p.slice(0, p.length - 2) + " мм рт.ст."
            }
        }
    }

    Devices.BusyRect {
        id: busyRect
        visible: !device.state
    }
}

