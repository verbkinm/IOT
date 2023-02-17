import QtQuick 2.9
import QtQuick.Controls 2.5
import Qt.labs.settings 1.1

Rectangle {
    id: componentRect

    readonly property string stateOnline: "online"
    readonly property string stateOffline: "offline"

    state: stateOffline

    width: 120
    height: 120

    border.width: 1
    border.color: Qt.rgba(0, 0, 0, 0.5)
    radius: 5

    MouseArea {
        anchors.fill: parent
        onClicked: {
            loaderDevice.setSource(createDeviceBy(client.deviceByName(model.name).id), {device: client.deviceByName(model.name)})
            loaderDevice.title = loaderDevice.objectName = client.deviceByName(model.name).aliasName
            appStack.push(loaderDevice)
        }
    }

    Image
    {
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
        text: (client != null) ? (client.deviceByName(model.name).aliasName) : ""
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

    Connections {
        target: client.deviceByName(name)
        function onSignalUpdate() { model.source = imageById(target.id) }
    }

    states: [
        State {
            name: stateOnline
            when: client.deviceByName(model.name).state
            PropertyChanges { target: componentRect; color: Qt.rgba(0, 1, 0, 0.1)}
        },
        State {
            name: stateOffline
            when: !client.deviceByName(model.name).state
            PropertyChanges { target: componentRect; color: Qt.rgba(1, 0, 0, 0.1)}
        }
    ]
    transitions: [
        Transition {
            to: stateOnline
            ColorAnimation { duration: 1000 }
        },
        Transition {
            to: stateOffline
            ColorAnimation { duration: 1000 }
        }
    ]

    function createDeviceBy(id)
    {
        if (id === 1)
            return "/Devices/Device_1.qml"
        else if (id === 2)
            return "/Devices/Device_2.qml"
        else if (id === 4)
            return "/Devices/Device_4.qml"
        else
            return "/Devices/Device_0.qml"
    }
}
