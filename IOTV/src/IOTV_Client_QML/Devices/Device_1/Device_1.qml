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
                model: device.readChannelLength

                Device_1_switch {
                    channel: model.index
                    btn.checked: device.readData(model.index) === "true"
                    btn.onClicked: {
                        device.setDataFromString(model.index, (btn.checked ? "true" : "false"))
                        btn.toggle()
                        wait()
                        timer.start()
                    }

                    SequentialAnimation {
                        id: anim
                        PropertyAnimation {
                            target: rec
                            property: "color"
                            from: Qt.rgba(0, 0, 0, 0)
                            to: Qt.rgba(0, 0, 255, 0.5)
                            duration: 200
                        }
                        PropertyAnimation  {
                            target: rec
                            property: "color"
                            from: Qt.rgba(0, 0, 255, 0.5)
                            to: Qt.rgba(0, 0, 0, 0)
                            duration: 200
                        }
                    }

                    Timer {
                        id: timer
                        interval: 10000
                        repeat: false
                        onTriggered: {
                            notWait()
                            loaderMainItem.setSource("qrc:/Notification.qml", {parent: appStack, text: desciptionLabel.text + "\nответ не получен"})
                        }
                    }

                    Connections {
                        target: device
                        function onSignalDataChanged(channel) {
                            if (channel !== model.index)
                                return

                            btn.checked = device.readData(channel) === "true"
                            anim.start()
                            timer.stop()
                        }
                    }
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
                                             text: "каналов чтения отсутствуют",
                                             standardButtons: Dialog.Ok})
            busyRect.visible = true
        }
    }

    Component.onDestruction: {
        console.log("Device 1 destruct: ", objectName)
    }

    Devices.BusyRect {
        id: busyRect
        visible: !device.state
    }

}

