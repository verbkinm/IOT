import QtQuick 2.9
import QtQuick.Controls 2.15
import Qt.labs.settings 1.1

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

        Rectangle {
            width: parent.width
            height: 20

            color: Qt.rgba(0, 0, 0, 0)
            Label {
                text: "Размер изображения:"
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
            height: 60

            color: Qt.rgba(100, 0, 0, 0)
            ComboBox {
                id: hostComboBox
                height: 60

                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 30
                }

                model: ["1280x720", "640x480", "320x240"]

                onCurrentIndexChanged: {
                    var w, h
                    if (currentIndex === 0)
                    {
                        w = 1280
                        h = 720
                    }
                    else if (currentIndex === 1)
                    {
                        w = 640
                        h = 480
                    }
                    else if (currentIndex === 2)
                    {
                        w = 320
                        h = 240
                    }
                    device.setDataFromString(1, w.toString())
                    device.setDataFromString(2, h.toString())
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 20

            color: Qt.rgba(0, 0, 0, 0)
            Label {
                text: "Качество изображения:"
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
                to: 100
                value: parseFloat(device.readData(3))

                anchors {
                    left: parent.left
                    right: valueBorder.left
                    rightMargin: 15
                    verticalCenter: parent.verticalCenter
                }
                onPressedChanged: {
                    if (!pressed)
                        device.setDataFromString(3, parseInt(value).toString())
                }
            }

            Label {
                id: valueBorder
                text: parseInt(sliderBorder.value).toString()
                width: 50

                anchors {
//                    rightMargin: 10
//                    left: sliderBorder.right
                    right: parent.right
                }
            }
        }
    }

    Connections {
        target: device
        function onSignalDataChanged(channel) {
//            if (channel === 1)
//                ;
//            else if (channel === 2)
//                ;
//            else if (channel === 3)
//            {
//                sliderBorder.value = parseFloat(device.readData(channel))
//            }
        }
    }

    Component.onCompleted: {
        console.log("Device settings 8 construct: ", objectName) - 1
    }

    Component.onDestruction: {
        console.log("Device settings 8 destruct: ", objectName)
    }
}
