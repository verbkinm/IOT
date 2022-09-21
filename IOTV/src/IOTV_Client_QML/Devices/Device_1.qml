import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4

Page {
    id: root

    //Ссылка на Device
    required property var device

    header: Rectangle {
        height: 64

        Button {
            id: info

            width: 48
            height: 48

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

            width: 48
            height: 48

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

            width: 48
            height: 48

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
        //        contentHeight: childrenRect.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Device_1_switch {
            device: root.device
            channel: 0
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
