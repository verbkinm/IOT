import QtQuick 2.9
import QtQuick.Controls 2.2
import Qt.labs.settings 1.1

Page {
    //Ссылка на Device
    required property var device

    id: root
    title: device.aliasName
    objectName: device.aliasName

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



            //Температура
            Rectangle {
                width: parent.width * 0.8
                height: 80

                border.width: 1
                border.color: "grey"
                radius: 10

                gradient: Gradient {
                    GradientStop {
                        position: 0.00;
                        color: "#e0d9d9";
                    }
                    GradientStop {
                        position: 1.00;
                        color: "#ffffff";
                    }
                }

                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                Image {
                    id: img_temperature
                    source: "qrc:/img/id_1/temperature.png"
                    fillMode: Image.PreserveAspectFit
                    height: parent.height * 0.80

                    anchors {
                        rightMargin: 20
                        right: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                }
                Label {
                    id: lbl_temperature
                    text: "0.00 °C"
                    font.pixelSize: 28
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        left: img_temperature.right
                        leftMargin: 20
                        verticalCenter: parent.verticalCenter
                    }
                }
            }

            //Влажность
            Rectangle {
                width: parent.width * 0.8
                height: 80

                border.width: 1
                border.color: "grey"
                radius: 10

                gradient: Gradient {
                    GradientStop {
                        position: 0.00;
                        color: "#e0d9d9";
                    }
                    GradientStop {
                        position: 1.00;
                        color: "#ffffff";
                    }
                }

                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                Image {
                    id: img_humidity
                    source: "qrc:/img/id_1/humidity.png"
                    fillMode: Image.PreserveAspectFit
                    height: parent.height * 0.70

                    anchors {
                        rightMargin: 20
                        right: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                }
                Label {
                    id: lbl_humidity
                    text: "0.00 %"
                    font.pixelSize: 28
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        left: img_humidity.right
                        leftMargin: 20
                        verticalCenter: parent.verticalCenter
                    }
                }
            }

            //Давление
            Rectangle {
                id: pressure
                width: parent.width * 0.8
                height: 80
                visible: false

                border.width: 1
                border.color: "grey"
                radius: 10

                gradient: Gradient {
                    GradientStop {
                        position: 0.00;
                        color: "#e0d9d9";
                    }
                    GradientStop {
                        position: 1.00;
                        color: "#ffffff";
                    }
                }

                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                Image {
                    id: img_pressure
                    source: "qrc:/img/id_1/pressure.png"
                    fillMode: Image.PreserveAspectFit
                    height: parent.height * 0.70

                    anchors {
                        rightMargin: 20
                        right: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                }
                Label {
                    id: lbl_pressure
                    text: "0.00"
                    font.pixelSize: 28
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        left: img_pressure.right
                        leftMargin: 20
                        //                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                }
                Label {
                    id: lbl_pressure_post
                    text: " мм рт.ст."
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    //                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        left: lbl_pressure.right
                        right: parent.right
                        //                        bottom: lbl_pressure.bottom
                        verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

    }

    Settings {
        id: setting
        category: device.name
        property alias name: root.title
    }

    Component.onCompleted: {
        console.log("Device 2 construct: ", objectName)
        if (device.readChannelLength === 3)
            pressure.visible = true

        timer.triggered()
    }

    Component.onDestruction: {
        console.log("Device 2 destruct: ", objectName)
    }

    BusyRect {
        visible: !device.state
    }

    Timer {
        id: timer
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            var t = device.readData(0)
            if (t > 0)
                t = "+" + t

            lbl_temperature.text = t.slice(0, t.length - 2) + " °C"

            var h = device.readData(1)
            lbl_humidity.text = h.slice(0, h.length - 2) + " %"

            if (device.readChannelLength === 3)
            {
                var p = device.readData(2)
                lbl_pressure.text = p.slice(0, p.length - 2)
            }

            fl.contentHeight = column.height + 50
        }
    }
}

