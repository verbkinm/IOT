import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    id: itemTimerType
    height: 50
    width: 400

    property int totalSeconds: 0
    property int h: 0
    property int m: 0
    property int s: 0

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
        standardButtons: Dialog.Save | Dialog.Cancel

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
        }

        onRejected: {
            hoursTumbler.currentIndex = h
            minutesTumbler.currentIndex = m
            secondsTumbler.currentIndex = s
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
        if (totalSeconds >= 60 * 60 * 24)
        {
            h = 23
            m = 59
            s = 59
        }
        else
        {
            h = totalSeconds / 3600;
            m = totalSeconds / 60 - h * 60;
            s = totalSeconds - m * 60 - h * 3600;
        }

        hoursTumbler.currentIndex = h
        minutesTumbler.currentIndex = m
        secondsTumbler.currentIndex = s
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

    function getTotalSeconds() {
        return hoursTumbler.currentIndex * 3600 + minutesTumbler.currentIndex * 60 + secondsTumbler.currentIndex
    }
}
