import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    id: itemAlarmType
    height: 50
    width: 400

    property string days: "0000000"
    property alias h: hours.currentIndex
    property alias m: minutes.currentIndex

    Button {
        text: "Настроить будильник"
        height: 50
        width: parent.width * 0.8
        anchors.centerIn: parent
        highlighted: true

        onClicked: {
            alarmSetting.visible = true

            console.log(days)
        }
    }

    Dialog {
        id: alarmSetting
        modal: true
        standardButtons: Dialog.Ok

        width: appStack.width
        height: item1.height + item2.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
                y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - alarmSetting.height / 2
        }

        Item {
            id: item1
            height: 180
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
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
                }

                Tumbler {
                    id: minutes
                    width: 80
                    height: 145
                    visibleItemCount: 3
                    model: 60
                    font.pixelSize: 18
                }
            }
            Rectangle {
                id: tumblerRect
                width: rowDays.width
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
                Repeater {
                    id: repeater
                    model: ["Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс"]
                    RoundButton{
                        checkable: true
                        text: modelData
                        highlighted: days[model.index] === "1"
                        onClicked: {
                            highlighted = !highlighted
                            days = days.slice(0, model.index) +  (highlighted ? "1" : "0") + days.slice(model.index + 1)
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: appStack
        function onHeightChanged() {
            alarmSetting.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - alarmSetting.height / 2
        }
        function onWidthChanged() {
            alarmSetting.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - alarmSetting.height / 2
        }
    }

    function time() {
        return hours.currentIndex + ":" + minutes.currentIndex
    }
}
