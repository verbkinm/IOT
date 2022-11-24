import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    //Ссылка на Device
    required property var device
    required property int channel
    property alias checked: button.checked
    property bool switchOn: false

    id: root
    height: 80

    color: Qt.rgba(0, 0, 0, 0)

    anchors{
        left: parent.left
        right: parent.right
    }

    Rectangle {
        height: parent.height
        width: parent.width * 0.8

        anchors {
            horizontalCenter: parent.horizontalCenter
        }

        border.color: "grey"
        border.width: 1
        radius: 15

        gradient: Gradient {
            GradientStop {
                position: 0.00;
                color: "#e0d9d9";
            }
            GradientStop {
                position: 1.00;
                color: "#ffffff";
            }
        }

        Switch{
            id: button
            height: 80

            checked: switchOn
            anchors{
                left: parent.left
                leftMargin: 30
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

        Label {
            id: userDescription

            height: button.height
            text: "Канал " + channel
            wrapMode: Text.WrapAnywhere
            elide: Text.ElideRight
            maximumLineCount: 2

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            antialiasing: true

            font.pixelSize: 18

            anchors{
                right: parent.right
                rightMargin: 10
                left: button.right
                verticalCenter: parent.verticalCenter
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    button.toggle()
                    button.toggled()
                }
            }
        }

        BusyRect {
            id: popup
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
            popup.close()
        }
    }

    Component.onDestruction: {
        console.log("Device 1_1 destruct:", objectName)
    }

    function clickButton()
    {
        //        player.source = "qrc:/audio/click.mp3"
        //        player.play()
        popup.open()
    }
}
