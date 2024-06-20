import QtQuick 2.9
import QtQuick.Controls 2.5

Rectangle {
    property alias text: txtConnection

    id: root
    anchors.fill: parent
    opacity: 1

    color: Qt.rgba(0, 0, 0, 0)

    readonly property string stateHide: "hide"
    readonly property string stateShow: "show"

    states: [
        State {
            name: stateHide
            when: !root.visible
            PropertyChanges { target: root; opacity: 0; visible: false }
        },
        State {
            name: stateShow
            when: root.visible
            PropertyChanges { target: root; opacity: 1; visible: true }
        }
    ]

    transitions: [
        Transition {
            to: stateHide
            ParallelAnimation{
                PropertyAnimation { target: root; property: "visible"; from: true; to: false; duration: 500 }
                PropertyAnimation { target: root; property: "opacity"; from: 1; to: 0; duration: 500 }
            }
        },
        Transition {
            to: stateShow
            PropertyAnimation { target: root; property: "opacity"; from: 0; to: 1; duration: 500 }
        }
    ]

    Text {
        id: txtConnection
        anchors.centerIn: parent
        font.pixelSize: 18
        wrapMode: Text.Wrap
    }

    function show()
    {
        root.state = stateShow
    }

    function hide()
    {
        root.state = stateHide
    }
}
