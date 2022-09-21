import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4

Rectangle {
    //Ссылка на Device
    required property var device
    required property int channel

    id: root

    height: 150
    border.color: "black"
    border.width: 1

    anchors{
        left: parent.left
        right: parent.right
        margins: 5
    }

    ToggleButton{
        id: button
        height: 90
        anchors{
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        text: checked ? "Вкл." : "Выкл."
        onClicked: {
            timer.stop()
            var state = button.checked === true ? "true" : "false"
//            button.checked = !button.checked
            device.setDataFromString(channel, state)
            timer.start()
        }
    }

    TextField {
        id: userDescription

        height: button.height
        text: "TEXT"

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        readOnly: true

        placeholderText: "Описание..."
        placeholderTextColor: "#ccc"

        font.pixelSize: 14

        anchors{
            right: parent.right
            rightMargin: 10
            left: button.right
            verticalCenter: parent.verticalCenter
        }
    }

    Timer {
        id: timer
        interval: 1500
        repeat: true
        running: true
        onTriggered: {
            button.checked = device.readData(channel) === "true" ? true : false
        }
    }
}
