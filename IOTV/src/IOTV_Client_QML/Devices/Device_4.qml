import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: root

    //Ссылка на Device
    required property var device

    property bool playSate: false
    property bool ledSate: false
    property bool repeateSate: false
    property int mode: 0

    header: Rectangle {
        height: 64

        Button {
            id: info

            width: 52
            height: 52

            anchors{
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: 10
            }
            display: AbstractButton.IconOnly
            icon {
                color: "transparent"
                source: "qrc:/img/info.png"
            }
            onClicked: {
                console.log("info")
                popup.open()
            }
            Popup {
                id: popup

                parent: root
                modal: true

                x: Math.round((parent.width - width) / 2)
                y: Math.round((parent.height - height) / 2)
                width: root.width
                height: root.height

                background: Rectangle {
                    color: Qt.rgba(255, 255, 255, 0.9)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        popup.close()
                    }
                }

                Label {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: device.description
                    font.pixelSize: 24
                    wrapMode: Text.Wrap
                }
            }
        }
        Button {
            id: debugMode

            width: 52
            height: 52

            anchors{
                right: info.left
                verticalCenter: parent.verticalCenter
                rightMargin: 10
            }
            display: AbstractButton.IconOnly
            icon {
                color: "transparent"
                source: "qrc:/img/debug.png"
            }

            onClicked: {
                var component = Qt.createComponent("/Devices/Device_0.qml");
                if (component.status === Component.Ready)
                {
                    var dev = device
                    var obj = component.createObject(appStack, {device: dev})
                    appStack.push(obj);
                }
            }
        }
        Button {
            id: readWrite

            width: 52
            height: 52

            anchors{
                right: debugMode.left
                verticalCenter: parent.verticalCenter
                rightMargin: 10
            }
            display: AbstractButton.IconOnly
            icon {
                color: "transparent"
                source: "qrc:/img/pen.png"
            }
        }
    }

    Flickable {
        id: fl
        width: root.width
        height: root.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Image {
            id: img
            source: playSate ? "qrc:/img/4_led.png" : "qrc:/img/id/4.png"
            width: parent.width - (parent.width * 100 / 10)
//            height: 150
            fillMode: Image.PreserveAspectFit

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
        }

        Row {
            anchors {
                top: img.bottom
                horizontalCenter: parent.horizontalCenter
//                left: parent.left
//                right: parent.right
            }
            spacing: 5

            Button {
                id: play
                width: 52
                height: 52
                text: "▶️"
                font.pixelSize: 18
                display: AbstractButton.TextOnly

                onClicked: {
                    if (playSate)
                    {
                        device.setDataFromString(0, "false")
                        text = "▶️"
//                        img.source = "qrc:/img/id/4.png"
                    }
                    else
                    {
                        device.setDataFromString(0, "true")
                        text = "⏸"
//                        img.source = "qrc:/img/4_led.png"
                    }
                    playSate = !playSate
                }
            }
            Button {
                id: led
                width: 52
                height: 52
                display: AbstractButton.IconOnly
                icon {
                    color: "transparent"
                    source: "qrc:/img/lamp_off.png"
                }
                onClicked: {
                    if (ledSate)
                    {
                        device.setDataFromString(1, "false")
                        icon.source = "qrc:/img/lamp_off.png"
                    }
                    else
                    {
                        device.setDataFromString(1, "true")
                        icon.source = "qrc:/img/lamp_on.png"
                    }
                    ledSate = !ledSate
                }
            }
            Button {
                id: autorepeate
                width: 52
                height: 52
                display: AbstractButton.IconOnly
                icon {
                    color: "transparent"
                    source: "qrc:/img/repeate.png"
                }
                onClicked: {
                    if (repeateSate)
                    {
                        device.setDataFromString(2, "false")
                        icon.source = "qrc:/img/repeate.png"
                    }
                    else
                    {
                        device.setDataFromString(2, "true")
                        icon.source = "qrc:/img/repeate.png"
                    }
                }
            }
            Button {
                id: mode1
                width: 52
                height: 52
                display: AbstractButton.TextOnly
                text: "1"
                font.pixelSize: 18
                onClicked: {
                    device.setDataFromString(3, "0")
                }
            }
            Button {
                id: mode2
                width: 52
                height: 52
                display: AbstractButton.TextOnly
                text: "2"
                font.pixelSize: 18
                onClicked: {
                    device.setDataFromString(3, "1")
                }
            }
            Button {
                id: mode3
                width: 52
                height: 52
                display: AbstractButton.TextOnly
                text: "3"
                font.pixelSize: 18
                onClicked: {
                    device.setDataFromString(3, "2")
                }
            }
        }
    }

    Component.onCompleted: {
        title = device.name
    }

    onVisibleChanged: {
        if (appStack.currentItem.title !== root.title)
            destroy()
    }
}

