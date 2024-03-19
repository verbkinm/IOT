import QtQuick 2.9
import QtQuick.Controls 2.5
import Qt.labs.settings 1.1
import "qrc:/Devices/BaseItem" as BaseItem

Item {
    readonly property string statePressed: "pressed"
    readonly property string stateRealesed: "released"

    id: root
    width: global_window.width / 2 - global_window.width * 0.1
    height: 110

    // Для загрузки единичного экземпляра устройства
    Loader {
        property string title
        id: loaderDevice
        source: ""
    }

    Canvas {
        id: shadow
        width: parent.width
        height: parent.height
        smooth: true

        //        z: -1
        onPaint: {
            var x = componentRect.x
            var y = componentRect.y
            var r = 5
            var w = componentRect.width
            var h = componentRect.height
            var ctx = getContext("2d")
            ctx.strokeStyle = "#aaa"
            ctx.beginPath()
            ctx.moveTo(x + r, y)
            ctx.arcTo(x + w, y, x + w, y + h, r)
            ctx.arcTo(x + w, y + h, x, y + h, r)
            ctx.arcTo(x, y + h, x, y, r)
            ctx.arcTo(x, y, x + w, y, r)
            ctx.closePath()
            ctx.shadowBlur = 5
            ctx.fill()
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            console.log("clicked")
            loaderDevice.setSource(createDeviceBy(client.deviceByName(
                                                      model.name).id), {
                                       "device": client.deviceByName(model.name)
                                   })
            loaderDevice.title = loaderDevice.objectName = Qt.binding(
                        function () {
                            return client.deviceByName(model.name).aliasName
                        })
            glob_deviceStackView.push(loaderDevice)
        }
    }

    states: [
        State {
            name: statePressed
            when: mouseArea.pressed
            PropertyChanges {
                target: componentRect
                scale: 0.95
            }
            PropertyChanges {
                target: shadow
                scale: 0.95
            }
        },
        State {
            name: stateRealesed
            when: !mouseArea.pressed
            PropertyChanges {
                target: componentRect
                scale: 1.0
            }
            PropertyChanges {
                target: shadow
                scale: 1.0
            }
        }
    ]

    transitions: [
        Transition {
            to: statePressed
            ParallelAnimation {
                PropertyAnimation {
                    target: componentRect
                    property: "scale"
                    from: 1.0
                    to: 0.95
                    duration: 50
                }
                PropertyAnimation {
                    target: shadow
                    property: "scale"
                    from: 1.0
                    to: 0.95
                    duration: 50
                }
            }
        },
        Transition {
            to: stateRealesed
            ParallelAnimation {
                PropertyAnimation {
                    target: componentRect
                    property: "scale"
                    from: 0.95
                    to: 1.0
                    duration: 100
                }
                PropertyAnimation {
                    target: shadow
                    property: "scale"
                    from: 0.95
                    to: 1.0
                    duration: 100
                }
            }
        }
    ]

    Rectangle {
        id: componentRect
        anchors.centerIn: parent

        width: root.width - 10
        height: 100

        color: Qt.rgba(255, 255, 255, 1)
        radius: 5

        Image {
            id: icon
            source: model.source
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
                topMargin: 10
            }
            width: 54
            height: 54
            fillMode: Image.PreserveAspectFit
        }

        Label {
            id: devName
            text: (client != null) ? (client.deviceByName(
                                          model.name).aliasName) : ""
            font.pixelSize: 16
            anchors {
                left: parent.left
                right: parent.right
                top: icon.bottom
                bottom: parent.bottom
                leftMargin: 10
                rightMargin: 10
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            elide: Text.ElideRight

            Settings {
                id: setting
                category: model.name
                property string name

                Component.onCompleted: {
                    if (this.name.length === 0)
                        this.name = model.name

                    var dev = client.deviceByName(model.name)
                    dev.aliasName = this.name
                }
            }
        }

        BaseItem.Led_state {
            deviceName: model.name
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: 10
                topMargin: 10
            }
        }

        Connections {
            target: client.deviceByName(name)
            function onSignalUpdate() {
                model.source = imageById(target.id)
            }
        }
    }

    function createDeviceBy(id) {
        switch(id)
        {
            case 1: return "/Devices/Device_1/Device_1.qml"
            case 2: return "/Devices/Device_2/Device_2.qml"
            case 4: return "/Devices/Device_4/Device_4.qml"
            case 5: return "/Devices/Device_5/Device_5.qml"
            case 8: return "/Devices/Device_8/Device_8.qml"
        }

        return "/Devices/Device_0.qml"
    }
}
