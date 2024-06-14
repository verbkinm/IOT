import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    required property var event

    id: itemAlarmType
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
            alarmSetting.visible = true
        }
    }

    Dialog {
        id: alarmSetting
        modal: true
        standardButtons: Dialog.Ok

        leftMargin: 15
        rightMargin: 15

        width: glob_eventStackView.width - leftMargin - rightMargin
        height: item1.height + item2.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
            {
                y = mapFromItem(glob_eventStackView, 0, 0).y + glob_eventStackView.height / 2 - alarmSetting.height / 2
            }
        }

        onAccepted: {
            event.totalSeconds = event.totalSeconds = hoursTumbler.currentIndex * 3600 + minutesTumbler.currentIndex * 60 + secondsTumbler.currentIndex
        }

        Item {
            id: item1
            width: parent.width
            height: 160
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

            Item {
                id: item2
                height: 80
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: item1.bottom
                anchors.leftMargin: 0
                Row {
                    id: rowDays
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 0
                    Repeater {
                        id: repeater
                        model: ["Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс"]
                        RoundButton{
                            width: 45
                            height: width
                            checkable: true
                            text: modelData
                            highlighted: event.days[model.index] === "1"
                            onClicked: {
                                highlighted = !highlighted
                                event.days = event.days.slice(0, model.index) + (highlighted ? "1" : "0") + event.days.slice(model.index + 1)
                                        //                                days_complete = days_complete.slice(0, model.index) + (highlighted ? "1" : "0") + days_complete.slice(model.index + 1)
                                        //                            days_complete[model.index] = (highlighted ? "1" : "0") не работает!
                            }
                        }
                    }
                }
            }
        }

        Connections {
            target: glob_eventStackView
            function onHeightChanged() {
                alarmSetting.y = mapFromItem(glob_eventStackView, 0, 0).y + glob_eventStackView.height / 2 - alarmSetting.height / 2
            }
            function onWidthChanged() {
                alarmSetting.y = mapFromItem(glob_eventStackView, 0, 0).y + glob_eventStackView.height / 2 - alarmSetting.height / 2
            }
        }
    }
}
