import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    required property var event

    id: itemAlarmType
    height: 50
    width: 400

    // days устанавливается из данных полученных с сервера. При сохранении на сервер так же считываются данные с days
    property string days: event.days
    // days_complete необходим для промежуточного хранения при открытии диалогового окна.
    property string days_complete: "0000000"

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
        standardButtons: Dialog.Save | Dialog.Cancel

        leftMargin: 15
        rightMargin: 15

        width: glob_eventStackView.width - leftMargin - rightMargin
        height: item1.height + item2.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
            {
                y = mapFromItem(glob_eventStackView, 0, 0).y + glob_eventStackView.height / 2 - alarmSetting.height / 2
                days_complete = days
            }
        }

        onAccepted: {
            days = days_complete
        }

        onRejected: {
            var childs = rowDays.children
            for (var i = 0; i < 7; ++i)
                childs[i].highlighted = days[i] === "1"

            days_complete = days
        }

        Item {
            id: item1
            height: 180
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            Row {
                anchors.centerIn: parent
                id: tumblerRow
                Tumbler {
                    id: hours
                    width: 80
                    height: 145
                    wheelEnabled: false
                    spacing: 0
                    visibleItemCount: 3
                    model: 24
                    font.pixelSize: 18

                    currentIndex: event.hour

                    Label {
                        font.pixelSize: 18
                        text: ":"
                        anchors {
                            verticalCenter: hours.verticalCenter
                            left: hours.right
                        }
                    }
                }

                Tumbler {
                    id: minutes
                    width: 80
                    height: 145
                    visibleItemCount: 3
                    model: 60
                    font.pixelSize: 18

                    currentIndex: event.minute
                }
            }
            Rectangle {
                id: tumblerRect
                width: parent.width * 0.9
                height: 60
                color: "#11000000"
                radius: 25
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
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
                        highlighted: days_complete[model.index] === "1"
                        onClicked: {
                            highlighted = !highlighted
                            days_complete = days_complete.slice(0, model.index) + (highlighted ? "1" : "0") + days_complete.slice(model.index + 1)
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

    function time() {
        return hours.currentIndex + ":" + minutes.currentIndex
    }
}
