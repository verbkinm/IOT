import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: root

    //Ссылка на Device
    required property var device

    header: DeviceHeader {
        id: headerPanel
    }

    Flickable {
        id: fl
        width: root.width
        height: root.height
        enabled: device.state

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
    }

    Component.onCompleted: {
        title = device.name

        if (device.readChannelLength !== device.writeChannelLength)
        {
            console.error("error device id 1")
            return
        }

        for (var i = 0; i < device.readChannelLength; i++)
        {
            var component = Qt.createComponent("Device_1_switch.qml");
            if (component.status === Component.Ready)
            {
                var obj = component.createObject(column, {device: root.device, channel: i, checked: root.device.readData(i) === "true" ? true : false})
                obj.objectName = objectName + "_channel_" + i
                fl.contentHeight = device.readChannelLength * obj.height + header.height
            }
        }
    }

    Component.onDestruction: {
        console.log("Device 1 destruct: ", objectName)
    }

    BusyRect {
        visible: !device.state
    }

}

