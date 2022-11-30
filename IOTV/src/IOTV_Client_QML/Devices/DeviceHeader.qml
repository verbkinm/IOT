import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {

    height: 64

    Text {
        text: device.state ? "online" : "offline"
        color: device.state ? "green" : "red"
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: settings.left
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Button {
        id: settings

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
            source: "qrc:/img/settings.png"
        }

        onClicked: {
            createComp("/Devices/Setting/Setting.qml", "_setting")
        }
    }

    Button {
        id: debugMode

        width: settings.width
        height: settings.height

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
            createComp("/Devices/Device_0.qml", "_debug")
        }
    }

    Button {
        id: info

        width: settings.width
        height: settings.height

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

    function createComp(fileName, postfix)
    {
        var find = false
        var pageObject

        for (var i = 0; i < appStack.children.length; i++)
        {
            if (appStack.children[i].objectName === name + postfix)
            {
                find = true
                pageObject = appStack.children[i]
                break
            }
        }

        if (find)
        {
            appStack.push(pageObject)
            return
        }

        var component = Qt.createComponent(fileName);
        if (component.status === Component.Ready)
        {
            var dev = device
            var obj = component.createObject(appStack, {device: dev})
            obj.objectName = name + postfix
            appStack.push(obj);
        }
    }
}
