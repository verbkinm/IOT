import QtQuick 2.9

Rectangle {
    required property var deviceName

    readonly property string stateOnline: "online"
    readonly property string stateOffline: "offline"

    state: stateOffline

    id: statusConnection
    width: 15
    height: 15

    border.width: 1
    border.color: Qt.rgba(0, 0, 0, 0.5)
    radius: 25

    color: Qt.rgba(255, 0, 0, 1)

    states: [
        State {
            name: stateOnline
            when: client.deviceByName(deviceName).state
            PropertyChanges {
                target: statusConnection
                color: Qt.rgba(0, 100, 0, 1)
            }
        },
        State {
            name: stateOffline
            when: !client.deviceByName(deviceName).state
            PropertyChanges {
                target: statusConnection
                color: Qt.rgba(255, 0, 0, 1)
            }
        }
    ]
    transitions: [
        Transition {
            to: stateOnline
            ColorAnimation {
                duration: 500
            }
        },
        Transition {
            to: stateOffline
            ColorAnimation {
                duration: 500
            }
        }
    ]
}
