import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    required property var event

    id: itemTimerType
    height: 50
    width: 400


    RoundButton {
        width: 64
        height: 64
        highlighted: true

        anchors {
            right: parent.right
            rightMargin: 20
        }

        Image {
            anchors.centerIn: parent
            source: "qrc:/img/settings_white.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit
        }

        onClicked: {
            timerSetting.visible = true
        }
    }

    Dialog {
        id: timerSetting
        modal: true
        standardButtons: Dialog.Ok

        leftMargin: 15
        rightMargin: 15

        width: glob_deviceStackView.width - leftMargin - rightMargin
        height: 240

        visible: false

        onVisibleChanged: {
            if (visible)
                y = mapFromItem(glob_deviceStackView, 0, 0).y + glob_deviceStackView.height / 2 - timerSetting.height / 2
        }

        onAccepted: {
            event.totalSeconds = hoursTumbler.currentIndex * 3600 + minutesTumbler.currentIndex * 60 + secondsTumbler.currentIndex
        }

        Item {
            id: item1
            anchors.fill: parent
            Row {
                id: rowTimer
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    topMargin: 20
                }

                Tumbler {
                    id: hoursTumbler
                    width: 80
                    height: 145
                    spacing: 0
                    visibleItemCount: 3
                    model: 24
                    font.pixelSize: 18

                    currentIndex: event.hour

                    Label {
                        font.pixelSize: 18
                        text: ":"
                        anchors {
                            verticalCenter: hoursTumbler.verticalCenter
                            left: hoursTumbler.right
                        }
                    }
                }

                Tumbler {
                    id: minutesTumbler
                    width: 80
                    height: 145
                    visibleItemCount: 3
                    model: 60
                    font.pixelSize: 18

                    currentIndex: event.minute

                    Label {
                        font.pixelSize: 18
                        text: ":"
                        anchors {
                            verticalCenter: minutesTumbler.verticalCenter
                            left: minutesTumbler.right
                        }
                    }
                }

                Tumbler {
                    id: secondsTumbler
                    width: 80
                    height: 145
                    visibleItemCount: 3
                    model: 60
                    font.pixelSize: 18

                    currentIndex: event.second
                }
            }

            Rectangle {
                id: tumblerRect
                width: parent.width * 0.9
                height: 60
                color: "#11000000"
                radius: 25
                anchors.verticalCenter: rowTimer.verticalCenter
                anchors.horizontalCenter: rowTimer.horizontalCenter
            }
        }
    }

    Component.onCompleted: {
        if (event.totalSeconds >= 60 * 60 * 24)
        {
            event.hour = 23
            event.minute = 59
            event.second = 59
        }
    }

    Connections {
        target: glob_deviceStackView
        function onHeightChanged() {
            timerSetting.y = mapFromItem(glob_eventStackView, 0, 0).y + glob_eventStackView.height / 2 - timerSetting.height / 2
        }
        function onWidthChanged() {
            timerSetting.y = mapFromItem(glob_eventStackView, 0, 0).y + glob_eventStackView.height / 2 - timerSetting.height / 2
        }
    }
}
