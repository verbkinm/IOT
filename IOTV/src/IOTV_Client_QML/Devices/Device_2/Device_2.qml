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

    Loader {
        id: chartsLoader
        property string title: "loaderTitle"

        onVisibleChanged: {
            if (this.visible === false)
            {
                chartsLoader.setSource("")
            }
        }
    }

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
                id: btnCharts
                width: 180
                height: 60

                highlighted: true
                text: "График"

                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    chartsLoader.setSource("Log_Charts.qml", {"device": device})
                    chartsLoader.title = "График " + device.aliasName
                    glob_deviceStackView.push(chartsLoader)
                }
            }

            onPositioningComplete: {
                fl.contentHeight = column.height + column.topPadding + column.spacing + overlayHeader.height + 15
            }
        }
    }

    Component.onCompleted: {
        console.log("Device 2 construct: ", title)
        fl.contentHeight = column.height + column.topPadding + column.spacing + overlayHeader.height
    }

    Component.onDestruction: {
        console.log("Device 2 destruct: ", title)
        chartsLoader.setSource("")
    }

    Devices.BusyRect {
        anchors.fill: parent
        visible: !device.state
    }
}

