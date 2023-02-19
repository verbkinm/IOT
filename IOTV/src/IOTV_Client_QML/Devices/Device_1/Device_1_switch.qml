import QtQuick 2.9
import QtQuick.Controls 2.2
import Qt.labs.settings 1.1

import "qrc:/Devices/" as Devices

Rectangle {
    property int channel
    property alias btn: button
    property alias rec: rectangle
    property alias desciptionLabel: userDescription

    id: root
    height: 80

    color: Qt.rgba(0, 0, 0, 0)

    anchors{
        left: parent.left
        right: parent.right
    }

    Rectangle {
        id: rectangle
        height: parent.height
        width: parent.width * 0.8

        color: Qt.rgba(0, 0, 0, 0)
        border.width: 1
        border.color: Qt.rgba(0, 0, 0, 0.1)
        radius: 5

        anchors {
            horizontalCenter: parent.horizontalCenter
        }

        Switch{
            id: button
            height: 80

            anchors{
                left: parent.left
                leftMargin: 30
                verticalCenter: parent.verticalCenter
            }

            onPositionChanged: {
                popup.visible = false
            }
        }

        Label {
            id: userDescription

            height: button.height
            text: "uesr description"
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            maximumLineCount: 2

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            antialiasing: true

            font.pixelSize: 18

            anchors{
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
            visible: false
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
        console.log("Device 1_1 construct:", objectName)
    }

    Component.onDestruction: {
        console.log("Device 1_1 destruct:", objectName)
    }

    function wait() {
        popup.open()
    }

    function notWait() {
        popup.close()
    }
}
