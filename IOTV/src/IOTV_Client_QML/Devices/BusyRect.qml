import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    width: parent.width
    height: parent.height
    z: 1
    color: Qt.rgba(0, 0, 0, 0.1)

    radius: 5

    MouseArea {
        anchors.fill: parent
    }

    BusyIndicator {
        id: indicator
        antialiasing: true
        anchors.centerIn: parent
        visible: true
        running: true
    }

    function open() {
        popup.visible = true
    }
    function close() {
        popup.visible = false
    }
}
