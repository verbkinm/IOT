import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    //Ссылка на Device
    required property var device
    required property int channel
    property alias checked: button.checked
    property bool switchOn: false

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

        border.color: "grey"
        border.width: 1

        Switch{
            id: button
            height: 90

            checked: switchOn
            anchors{
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            onToggled: {
                var state = button.checked ? "true" : "false"
                button.checked = !button.checked

                if (switchOn)
                    device.setDataFromString(channel, "false")
                else
                    device.setDataFromString(channel, "true")

                clickButton()
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
            var btnCheck = device.readData(channel) === "true" ? true : false
            if (btnCheck !== switchOn)
            {
                popup.open()
                popupTimer.start()
            }
            else
                button.checked = switchOn

            switchOn = btnCheck
        }
    }

    Timer {
        id: popupTimer
        interval: timer.interval * 1.25
        running: true
        repeat: false
        onTriggered: {
            console.log("popupTimer")
            popup.close()
        }
    }

    Component.onDestruction: {
        console.log("Device 1_1 destruct:", objectName)
    }

    BusyRect {
        id: popup
    }

    function clickButton()
    {
        //        player.source = "qrc:/audio/click.mp3"
        //        player.play()
        popup.open()
    }
}
