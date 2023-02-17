import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/" as Devices

Page {
    //Ссылка на Device
    required property var device

    id: root
    title: device.aliasName
    objectName: device.aliasName

    header: Devices.DeviceHeader {
        id: headerPanel
    }

    Flickable {
        id: fl
        width: root.width
        height: root.height
        enabled: device.state

        contentHeight: column.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            spacing: 15
            topPadding: 15
        }
        Repeater {
            model: device.readChannelLength

            Device_1_switch {
                device: root.device
                channel: model.index
                checked: root.device.readData(model.index) === "true" ? true : false
            }
        }
    }

    Component.onCompleted: {
        console.log("Device 1 construct: ", objectName)
//        if (device.readChannelLength !== device.writeChannelLength)
//        {
//            console.error("error device id 1")
//            return
//        }

//        for (var i = 0; i < device.readChannelLength; i++)
//        {
//            var component = Qt.createComponent("Device_1_switch.qml");
//            if (component.status === Component.Ready)
//            {
//                var obj = component.createObject(column, {device: root.device, channel: i, checked: root.device.readData(i) === "true" ? true : false})
//                fl.contentHeight = device.readChannelLength * obj.height + header.height
//            }
//        }
    }

    Component.onDestruction: {
        console.log("Device 1 destruct: ", objectName)
    }

    Devices.BusyRect {
        visible: !device.state
    }

}

