import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    //Ссылка на Device
    required property var device
    required property int channel
    property alias checked: button.checked

    id: root
    height: 150

    anchors{
        left: parent.left
        right: parent.right
    }

    Rectangle {
        anchors {
            fill: parent
            bottomMargin: 15
            leftMargin: 10
            rightMargin: 10
        }

//        Rectangle {
//            color: device.state ? Qt.rgba(0.5, 0.5, 0.5, 0) : Qt.rgba(0.5, 0.5, 0.5, 0.2)
//            anchors.fill: parent
//            z: 1
//        }

        border.color: "grey"
        border.width: 1

        Switch{
            id: button
            height: 90
            anchors{
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            onToggled: {
                timer.stop()
                var state = button.checked ? "true" : "false"
                checked = !checked
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
    }

    Timer {
        id: timer
        interval: 500
        repeat: true
        running: true
        onTriggered: {
            button.checked = device.readData(channel) === "true" ? true : false
        }
    }

    Component.onDestruction: {
        console.log("Device 1_1 destruct:", objectName)
    }

//    Component.onDestruction: {
//        console.log("Device 1 destruct")
//    }

//    onVisibleChanged: {
//        console.log(appStack.children)
//    }
}
