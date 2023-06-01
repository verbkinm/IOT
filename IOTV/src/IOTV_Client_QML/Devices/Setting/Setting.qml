import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "qrc:/Devices/" as Devices

Page {    
    //Ссылка на Device
    required property var device

    id: root
    title: device.aliasName
    objectName: device.aliasName

    Flickable {
        id: fl
        width: root.width
        height: root.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            spacing: 15
            topPadding: 15

            MainSetting {
                deviceName: device.name
            }
        }
    }

//    Devices.BusyRect {
//        id: busyRect
//        anchors.fill: parent
//        visible: !device.state
//    }

    Component.onCompleted: {
        console.log("Setting construct: ", objectName)
        var component = Qt.createComponent("Device/Device_" + device.id + ".qml");
        if (component.status === Component.Ready)
        {
            var obj = component.createObject(column, {device: root.device})
            obj.objectName = objectName + "_setting_channels"
            fl.contentHeight = obj.height
        }
    }

    Component.onDestruction: {
        console.log("Setting destruct: ", objectName)
    }
}
