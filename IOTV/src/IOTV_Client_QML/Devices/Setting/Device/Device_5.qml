import QtQuick 2.9
import QtQuick.Controls 2.15
 import QtCore

Rectangle {
    //Ссылка на Device
    required property var device

    id: componentRect
    width: parent.width
    height: 180

    color: Qt.rgba(0, 0, 0, 0)

    Column {
        width: parent.width
        spacing: 15

        leftPadding: 15
        rightPadding: 15

        TextField {
            id: txt
            height: 52
            width: parent.width - parent.leftPadding - parent.rightPadding
            antialiasing: true
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            placeholderText: "Имя канала"
        }

        Rectangle {
            width: parent.width
            height: 20

            color: Qt.rgba(0, 0, 0, 0)
            Label {
                text: "Порог срабатывания:"
                font.pixelSize: 14
                elide: Text.ElideRight

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    leftMargin: 15
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 20
            color: Qt.rgba(0, 0, 0, 0)

            Slider {
                id: sliderBorder
                from: 0
                to: 255
                value: parseFloat(device.readData(1))
                //                width: 240
                anchors {
                    left: parent.left
                    right: valueBorder.left
                    verticalCenter: parent.verticalCenter
                    //                    leftMargin: 15
                }
                onPressedChanged: {
                    if (!pressed)
                     device.setDataFromString(1, parseInt(value).toString())
                }
            }

            Label {
                id: valueBorder
                text: parseInt(sliderBorder.value).toString()
                width: 50

                anchors {
                    rightMargin: 15
                    right: parent.right
                }
            }
        }
    }

    Connections {
        target: device
        function onSignalDataChanged(channel) {
            if (channel === 1) {
                if (!sliderBorder.pressed)
                    sliderBorder.value = parseFloat(device.readData(1))
            }
        }
    }

    Component.onCompleted: {
        console.log("Device settings 5 construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Device settings 5 destruct: ", objectName)
    }

    function getRange(min, max) {
        var arr = []
        var j = 0
        for (var i = min; i <= max; ++i) {
            arr[j] = i
            ++j
        }

        return arr
    }

    Settings {
        id: settings
        category: device.name + "_channel_" + 0
        property alias name: txt.text
    }
}
