import QtQuick 2.9
import QtQuick.Controls 2.2
import Qt.labs.settings 1.1

import "qrc:/Devices/" as Devices

Rectangle {
    required property var device
    required property int channel

    //    signal signalDataRX(var data, int channel)
    id: root
    width: parent.width * 0.8
    height: 80

    color: Qt.rgba(255, 255, 255, 1)

    anchors {
        left: parent.left
        right: parent.right
    }

    Canvas {
        id: shadow
        width: parent.width
        height: parent.height
        smooth: true

        onPaint: {
            var x = rectangle.x
            var y = rectangle.y
            var r = 5
            var w = rectangle.width
            var h = rectangle.height
            var ctx = getContext("2d")
            ctx.strokeStyle = "#aaa"
            ctx.beginPath()
            ctx.moveTo(x + r, y)
            ctx.arcTo(x + w, y, x + w, y + h, r)
            ctx.arcTo(x + w, y + h, x, y + h, r)
            ctx.arcTo(x, y + h, x, y, r)
            ctx.arcTo(x, y, x + w, y, r)
            ctx.closePath()
            ctx.shadowBlur = 3
            ctx.fill()
        }
    }

    Rectangle {
        id: rectangle
        height: parent.height - 10
        width: parent.width * 0.8 - 10

        color: Qt.rgba(255, 255, 255, 1)
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
                                         "text": text + "\nответ не получен"
                                     })
        }
    }

    SequentialAnimation {
        id: anim
        PropertyAnimation {
            target: rectangle
            property: "color"
            from: Qt.rgba(255, 255, 255, 1)
            to: Qt.rgba(0, 0, 255, 0.5)
            duration: 200
        }
        PropertyAnimation {
            target: rectangle
            property: "color"
            from: Qt.rgba(0, 0, 255, 0.5)
            to: Qt.rgba(255, 255, 255, 1)
            duration: 200
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
