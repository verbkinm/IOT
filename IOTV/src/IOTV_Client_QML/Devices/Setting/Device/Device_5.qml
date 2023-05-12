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

    //    border.width: 1
    //    border.color: "green"

    Column {
        width: parent.width
        spacing: 15

        Rectangle
        {
            color: Qt.rgba(0, 0, 0, 0)

            //            border.width: 1
            //            border.color: "red"

            width: parent.width
            height: 40

            Label {
                id: lbl
                wrapMode: Text.Wrap
                text: "Канал : " + 0
                font.pixelSize: 14
                elide: Text.ElideRight

                anchors{
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    leftMargin: 15
                    rightMargin: 15
                }
            }

            TextField {
                id: txt
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter

                anchors {
                    left: lbl.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    leftMargin: 15
                    rightMargin: 15
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 20

            color: Qt.rgba(0, 0, 0, 0)

            //            border.color: "yellow"
            //            border.width: 1

            Label {
                text: "Дата, время:"
                font.pixelSize: 14
                elide: Text.ElideRight

                anchors{
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    leftMargin: 15
                }
            }
        }

        Rectangle {
            id: rectTime
            width: parent.width
            height: 80

            color: Qt.rgba(0, 0, 0, 0)

            //            border.color: "red"
            //            border.width: 1

            FontMetrics {
                id: fontMetrics
            }

            Component {
                id: delegateComponent

                Label {

                    function formatText(count, modelData) {
                        var data = count === 12 ? modelData + 1 : modelData;
                        return data.toString().length < 2 ? "0" + data : data;
                    }

                    text: formatText(Tumbler.tumbler.count, modelData)
                    opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: fontMetrics.font.pixelSize * 1.25
                }
            }

            Tumbler {
                id: hoursTumbler
                model: 24
                delegate: delegateComponent
                visibleItemCount: 3
                height: parent.height
                anchors {
                    right: minutesTumbler.left
                }
                onCurrentIndexChanged: {
                    console.log(currentIndex)
                }
            }

            Tumbler {
                id: minutesTumbler
                model: 60
                delegate: delegateComponent
                visibleItemCount: 3
                height: parent.height
                anchors {
                    centerIn: parent
                }
            }

            Tumbler {
                id: secondsTumbler
                model: 60
                delegate: delegateComponent
                visibleItemCount: 3
                height: parent.height
                anchors {
                    left: minutesTumbler.right
                }
            }

            Rectangle {
                width: parent.width
                height: parent.height / 3

                color: Qt.rgba(0, 0, 0, 0)
                //                border.color: "blue"
                //                border.width: 1

                anchors {
                    centerIn: parent
                }

            }
            MouseArea {
                anchors.fill: parent
            }
        }

        Rectangle {
            width: parent.width
            height: 80

            color: Qt.rgba(0, 0, 0, 0)

            //            border.color: "green"
            //            border.width: 1

            Tumbler {
                id: dayTumbler
                model: getRange(1, 31)
                delegate: delegateComponent
                visibleItemCount: 3
                height: parent.height
                anchors {
                    right: monthTumbler.left
                }
                onCurrentIndexChanged: {
                    console.log(currentIndex)
                }
            }

            Tumbler {
                id: monthTumbler
                model: getRange(0, 11)
                delegate: delegateComponent
                visibleItemCount: 3
                height: parent.height
                anchors {
                    centerIn: parent
                }
                onCurrentIndexChanged: {
                    console.log(currentIndex)
                }
            }

            Tumbler {
                id: yearTumbler
                model: getRange(2000, 2099)
                delegate: delegateComponent
                visibleItemCount: 3
                height: parent.height
                anchors {
                    left: monthTumbler.right
                }
                //                onCurrentIndexChanged: {
                //                    device.setDataFromString(8, currentIndex + 2000)
                //                    console.log(currentIndex)
                //                }
            }
            MouseArea {
                anchors.fill: parent
            }
        }

        Button {
            id: btnSystemDateTime
            text: "Установить\nсистемное время"

            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                var dt = new Date();

                device.setDataFromString(2, dt.getSeconds())
                device.setDataFromString(3, dt.getMinutes())
                device.setDataFromString(4, dt.getHours())

                device.setDataFromString(5, dt.getDay())
                device.setDataFromString(6, dt.getDate())
                device.setDataFromString(7, dt.getMonth())
                device.setDataFromString(8, dt.getFullYear() - 2000)

                secondsTumbler.currentIndex = dt.getSeconds()
                minutesTumbler.currentIndex = dt.getMinutes()
                hoursTumbler.currentIndex = dt.getHours()
                dayTumbler.currentIndex = dt.getDate() - 1
                monthTumbler.currentIndex = dt.getMonth() - 1
                yearTumbler.currentIndex = dt.getFullYear() - 2000
            }
        }
    }

    Connections {
        target: device
        function onSignalDataChanged(channel) {
            if (channel === 2)
                secondsTumbler.currentIndex = Number(device.readData(2))
            else if (channel === 3)
                minutesTumbler.currentIndex = Number(device.readData(3))
            else if (channel === 4)
                hoursTumbler.currentIndex = Number(device.readData(4))
            else if (channel === 6)
                dayTumbler.currentIndex = Number(device.readData(6)) - 1
            else if (channel === 7)
                monthTumbler.currentIndex = Number(device.readData(7)) - 1
            else if (channel === 8)
                yearTumbler.currentIndex = Number(device.readData(8))

        }
    }

    Component.onCompleted: {
        console.log("Device settings 5 construct: ", objectName)
        secondsTumbler.currentIndex = Number(device.readData(2))
        minutesTumbler.currentIndex = Number(device.readData(3))
        hoursTumbler.currentIndex = Number(device.readData(4))
        dayTumbler.currentIndex = Number(device.readData(6)) - 1
        monthTumbler.currentIndex = Number(device.readData(7)) - 1
        yearTumbler.currentIndex = Number(device.readData(8))
    }

    Component.onDestruction: {
        console.log("Device settings 5 destruct: ", objectName)
    }

    function getRange(min, max){
        var arr = []
        for (var i = min, j = 0; i <= max; i++, j++)
            arr[j] = i

        return arr
    }

    Settings {
        id: settings
        category: device.name + "_channel_" + 0
        property alias name: txt.text
    }
}
