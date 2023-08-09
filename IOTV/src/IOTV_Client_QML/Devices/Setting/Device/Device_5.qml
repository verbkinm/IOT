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

        TextField {
            id: txt
            height: 52
            width: parent.width - parent.leftPadding - parent.rightPadding
            antialiasing: true
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            placeholderText: "Имя канала"
        }

        TextField {
            id: dateTime
            readOnly: true
            height: 52
            antialiasing: true
            font.pixelSize: 14
            placeholderText: "Время и дата"
            horizontalAlignment: Text.AlignHCenter

            anchors {
                left: parent.left
                right: parent.right
                leftMargin: 15
                rightMargin: 15
            }

            property int year: 0
            property int month: 1
            property int day: 1
            property int hours: 0
            property int minutes: 0
            property int seconds: 0

            text: addZero(hours) + ":" + addZero(minutes) + ":" + addZero(
                      seconds) + " " + addZero(day) + "/" + addZero(
                      month) + "/20" + addZero(year)
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

        Button {
            id: btnSystemDateTime
            text: "Установить системное время"
            height: 60
            font.pixelSize: 18
            highlighted: true

            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                var dt = new Date()

                device.setDataFromString(2, dt.getSeconds())
                device.setDataFromString(3, dt.getMinutes())
                device.setDataFromString(4, dt.getHours())

                device.setDataFromString(5, dt.getDay())
                device.setDataFromString(6, dt.getDate())
                device.setDataFromString(7, dt.getMonth() + 1)
                device.setDataFromString(8, dt.getFullYear() - 2000)

                dateTime.seconds = dt.getSeconds()
                dateTime.minutes = dt.getMinutes()
                dateTime.hours = dt.getHours()
                dateTime.day = dt.getDate()
                dateTime.month = dt.getMonth() + 1
                dateTime.year = dt.getFullYear() - 2000
            }
        }
    }

    Connections {
        target: device
        function onSignalDataChanged(channel) {
            if (channel === 1) {
                if (!sliderBorder.pressed)
                    sliderBorder.value = parseFloat(device.readData(1))
            } else if (channel === 2)
                dateTime.seconds = Number(device.readData(2))
            else if (channel === 3)
                dateTime.minutes = Number(device.readData(3))
            else if (channel === 4)
                dateTime.hours = Number(device.readData(4))
            else if (channel === 6)
                dateTime.day = Number(device.readData(6))
            else if (channel === 7)
                dateTime.month = Number(device.readData(7))
            else if (channel === 8)
                dateTime.year = Number(device.readData(8))
        }
    }

    Component.onCompleted: {
        console.log("Device settings 5 construct: ", objectName)
        dateTime.seconds = Number(device.readData(2))
        dateTime.minutes = Number(device.readData(3))
        dateTime.hours = Number(device.readData(4))
        dateTime.day = Number(device.readData(6))
        dateTime.month = Number(device.readData(7))
        dateTime.year = Number(device.readData(8))
        //        secondsTumbler.currentIndex =
        //        minutesTumbler.currentIndex = Number(device.readData(3))
        //        hoursTumbler.currentIndex =
        //        dayTumbler.currentIndex =
        //        monthTumbler.currentIndex =  - 1
        //        yearTumbler.currentIndex =
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

    function addZero(val) {
        if (val < 10)
            return "0" + val

        return val
    }

    Settings {
        id: settings
        category: device.name + "_channel_" + 0
        property alias name: txt.text
    }
}
