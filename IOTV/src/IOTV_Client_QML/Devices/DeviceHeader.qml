import QtQuick 2.9
import QtQuick.Controls

Rectangle {
    height: 64
    color: Qt.rgba(0, 0, 0, 0)

    Text {
        id: stateDev
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
        Image {
            anchors.centerIn: parent
            source: "qrc:/img/settings.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit  // ensure it fits
        }

        //        display: AbstractButton.IconOnly
        //        icon {
        //            color: "transparent"
        //            source: "qrc:/img/settings.png"
        //            height: 50
        //            width: 50
        //        }

        onClicked: {
            loaderDebug.objectName = device.aliasName + "_setting"
            loaderDebug.setSource("/Devices/Setting/Setting.qml", {device: device})
            appStack.push(loaderDebug)
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
        Image {
            anchors.centerIn: parent
            source: "qrc:/img/debug.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit  // ensure it fits
        }
//        display: AbstractButton.IconOnly
//        icon {
//            color: "transparent"
//            source: "qrc:/img/debug.png"
//        }

        onClicked: {
            loaderDebug.objectName = device.aliasName + "_debug"
            loaderDebug.setSource("/Devices/Device_0.qml", {device: device})
            appStack.push(loaderDebug)
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
        Image {
            anchors.centerIn: parent
            source: "qrc:/img/info.png"
            height: 22
            width: 22
            fillMode: Image.PreserveAspectFit  // ensure it fits
        }
//        display: AbstractButton.IconOnly
//        icon {
//            color: "transparent"
//            source: "qrc:/img/info.png"
//        }
        onClicked: {
            loaderMainItem.setSource("qrc:/DialogShared.qml",
                                     {parent: appStack,
                                         visible: true,
                                         title: "Описание",
                                         standardButtons: Dialog.Ok,
                                         text: device.description})
        }
    }

    Loader {
        property string title: device.aliasName

        id: loaderDebug
        objectName: "debug"
        source: ""
    }
}
