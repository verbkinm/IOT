import QtQuick 2.9
import QtQuick.Controls 2.2
import Qt.labs.settings 1.1

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem

BaseItem.ItemShadow {
    required property var device
    required property int channel

    //    signal signalDataRX(var data, int channel)
    id: root
    width: parent.width
    height: 80

    anchors {
        left: parent.left
        right: parent.right
    }

    Rectangle {
        // для тени id должно называться rectangle
        id: rectangle
        height: 70
        width: parent.width * 0.8

        radius: 5

        anchors {
            centerIn: parent
        }

        Switch {
            id: button
            height: 80
            checked: device.readData(channel) === "true"

            anchors {
                left: parent.left
                leftMargin: 30
                verticalCenter: parent.verticalCenter
            }

            onPositionChanged: {
                popup.visible = false
            }

            onClicked: {
                device.setDataFromString(channel,
                                         (button.checked ? "true" : "false"))
                button.toggle()
                wait()
                timer.start()
            }
        }

        Label {
            id: userDescription

            height: button.height
            text: "Канал " + channel
            wrapMode: Text.WordWrap

            elide: Text.ElideRight
            maximumLineCount: 2

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            antialiasing: true

            font.pixelSize: 18

            anchors {
                right: parent.right
                rightMargin: 10
                left: button.right
                verticalCenter: parent.verticalCenter
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    button.toggle()
                    button.clicked()
                }
            }
        }

        Devices.BusyRect {
            id: popup
            anchors.fill: parent
            visible: false
        }
    }

    Connections {
        target: device
        function onSignalDataChanged(ch) {
            if (ch === channel) {
                button.checked = device.readData(channel) === "true"
                anim.start()
                timer.stop()
                //                signalDataRX(button.checked, ch)
            }
        }
    }

    Timer {
        id: timer
        interval: 5000
        repeat: false
        onTriggered: {
            notWait()
            loaderMainItem.setSource("qrc:/Notification.qml", {
                                         "parent": appStack,
                                         "text": userDescription.text + "\nответ не получен"
                                     })
        }
    }

    SequentialAnimation {
        id: anim
        ParallelAnimation {
            PropertyAnimation {
                target: rectangle
                property: "color"
                from: Qt.rgba(255, 255, 255, 1)
                to: Qt.rgba(0, 0, 100, 0.5)
                duration: 200
            }
            PropertyAnimation {
                target: userDescription
                property: "color"
                from: Qt.rgba(0, 0, 0, 1)
                to: Qt.rgba(255, 255, 255, 1)
                duration: 200
            }
        }
        ParallelAnimation {
            PropertyAnimation {
                target: rectangle
                property: "color"
                from: Qt.rgba(0, 0, 100, 0.5)
                to: Qt.rgba(255, 255, 255, 1)
                duration: 200
            }
            PropertyAnimation {
                target: userDescription
                property: "color"
                from: Qt.rgba(255, 255, 255, 1.0)
                to: Qt.rgba(0, 0, 0, 1.0)
                duration: 200
            }
        }
    }

    Settings {
        id: setting
        category: device.name + "_channel_" + channel
        property alias name: userDescription.text
    }

    Component.onCompleted: {
        if (setting.name.length === 0)
            setting.name = "Канал " + channel
        console.log("Device 1_", channel, " construct:", objectName)
    }

    Component.onDestruction: {
        console.log("Device 1_", channel, " destruct:", objectName)
    }

    function wait() {
        popup.open()
    }

    function notWait() {
        popup.close()
    }
}
