import QtQuick 2.9
import QtQuick.Controls 2.2
import "../"    // DialogShared.qml

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
            loaderItem.objectName = device.aliasName + "_setting"
            loaderDebug.setSource("/Devices/Setting/Setting.qml", {device: device})
            appStack.push(loaderItem)
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
            loaderItem.objectName = device.aliasName + "_debug"
            loaderDebug.setSource("/Devices/Device_0.qml", {device: device})
            appStack.push(loaderItem)
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
            dialogInfo.open()
        }
    }

    Item {
        property string title: device.aliasName

        id: loaderItem
        objectName: "debug"

        Loader {
            id: loaderDebug
            anchors.fill: parent
            source: ""
        }
    }

    DialogShared
    {
        id: dialogInfo
        parent: appStack
        title: "Описание"
        standardButtons: Dialog.Ok
        text: device.description
    }
}
