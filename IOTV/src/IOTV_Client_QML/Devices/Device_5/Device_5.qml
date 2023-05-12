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

    Connections {
        target: device
        function onSignalDataChanged(channel) {
            if (channel === 0)
            {
                btn_id5.btn.checked = device.readData(0) === "true"
                anim.start()
                timer.stop()
            }
        }
    }

    Devices.BusyRect {
        id: busyRect
        visible: !device.state
    }
}

