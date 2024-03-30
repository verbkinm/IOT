import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/BaseItem" as BaseItem
import "qrc:/Devices/" as Devices


Page {
    //Ссылка на Device
    required property var device

    property list<bool> waitEndLogData: [false, false, false]

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

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            spacing: 15

            anchors {
                top: parent.top
                topMargin: 15
            }

            BaseItem.MeteoBlock {
                id: meteoBlock
                width: parent.width
                device: root.device
                channelTemperature: 0
                channelHumidity: 1
                channelPressure: 2
            }

            Button {
                id: queryLog
                width: parent.width
                height: 80

                onClicked: {
                    var startInterval = new Date(2024, 2, 29, 14, 57, 0, 0).getTime();
                    var endInterval = new Date(2025, 2, 29, 17, 0, 0, 0).getTime()
                    var interval = 1000
                    var ch = 0
                    var flags = 0

                    waitEndLogData = [true, true, true]

                    device.signalQueryLogData(startInterval, endInterval, interval, 0, flags)
                    device.signalQueryLogData(startInterval, endInterval, interval, 1, flags)
                    device.signalQueryLogData(startInterval, endInterval, interval, 2, flags)
                }
            }

            onPositioningComplete: {
                fl.contentHeight = column.height + column.topPadding + column.spacing + overlayHeader.height + 15

            }
        }
    }

    Connections {
        target: device
        function onSignalResponceLogData(data, timeMS, channelNumber, flags) {
            console.log(timeMS, channelNumber, flags, data)

            if (data === "" && channelNumber < waitEndLogData.length)
            {
                waitEndLogData[channelNumber] = false
            }

            var allDone = false
            for (var i = 0; i < waitEndLogData.length; i++)
            {
                if (waitEndLogData[i] === true)
                    return
            }

            console.log("all done")
        }
    }

    Component.onCompleted: {
        console.log("Device 2 construct: ", title)
        fl.contentHeight = column.height + column.topPadding + column.spacing + overlayHeader.height
    }

    Component.onDestruction: {
        console.log("Device 2 destruct: ", title)
    }

    Devices.BusyRect {
        anchors.fill: parent
        visible: !device.state
    }
}

