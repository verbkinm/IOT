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


    Dialog {
        id: dialogInfo
        parent: root
        modal: true

        title: "Описание"
        standardButtons: Dialog.Ok

        width: parent.width * 0.8
        height: parent.height * 0.5
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)

        Label {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: device.description
            font.pixelSize: 18
            wrapMode: Text.Wrap
        }
    }

//    function createComp(fileName, postfix)
//    {
//        var find = false
//        var pageObject

//        for (var i = 0; i < appStack.children.length; i++)
//        {
//            if (appStack.children[i].objectName === name + postfix)
//            {
//                find = true
//                pageObject = appStack.children[i]
//                break
//            }
//        }

//        if (find)
//        {
//            appStack.push(pageObject)
//            return
//        }

//        var component = Qt.createComponent(fileName);
//        if (component.status === Component.Ready)
//        {
//            var dev = device
//            var obj = component.createObject(appStack, {device: dev})
//            obj.objectName = name + postfix
//            appStack.push(obj);
//        }
//    }
}
