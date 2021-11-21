import QtQuick 2.0
import QtQuick.Controls 2.15
import io.qt.Backend_Device 1.0

Rectangle {
    property alias name: title.text
    property alias device: backend_device

    signal info()

    id: root
    anchors.fill: parent

    Backend_Device {
        id: backend_device
        onStateChanged:
        {
            root.color = state ? "#2000FF00" : "#20FF0000"
            root.parent.setState(state)
        }
    }

    border.width: 1
    border.color: "black"
    color: "#20FF0000"
    radius: 5
    smooth: true

    Label {
        id: title
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: "Неизвестное устройство"
    }

    MouseArea {
        width: 36
        height: 36
        anchors.right: parent.right

        onClicked: {
            info()
        }

        Image {
            id: img
            source: "qrc:/img/info"
            sourceSize: Qt.size(36, 36)
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}



