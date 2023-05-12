import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/BaseItem" as BaseItem
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

    //    clip: true

    Flickable {
        id: fl
        width: root.width
        height: root.height
        enabled: device.state

        contentHeight: meteoBlock.height + 50

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        BaseItem.MeteoBlock {
            id: meteoBlock
            anchors.fill: parent
            device: root.device

            channelTemperature: 0
            channelHumidity: 1
            channelPressure: 2
        }
    }

    Component.onCompleted: {
        console.log("Device 2 construct: ", title)
    }

    Component.onDestruction: {
        console.log("Device 2 destruct: ", title)
    }



    Devices.BusyRect {
        visible: !device.state
    }
}

