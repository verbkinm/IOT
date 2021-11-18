import QtQuick 2.0
import QtQuick.Controls 2.15
import io.qt.Backend_Device 1.0

Rectangle {
    id: root
    property alias name: title.text
    property alias device: backend_device

    Backend_Device {
        id: backend_device

        onStateChanged:
        {
            root.color = state ? "#2000FF00" : "#20FF0000"
        }
    }

    signal info();

    border.width: 1
    border.color: "black"
    color: "#20FF0000"
    radius: 5
    smooth: true

    MouseArea {
        anchors.fill: parent
        onClicked: {
            info()
            root.parent.showInfo(parent)
        }

        Label {
            id: title
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: "Неизвестное устройство"
        }

        Image {
            id: img
            source: "qrc:/img/id_0"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: title.bottom
            anchors.topMargin: 5
        }
        //        Image {
        //            id: state
        //            source: "qrc:/img/led_off"
        //            sourceSize.width: 24
        //            sourceSize.height: 24
        //            anchors.right: parent.right
        //            anchors.topMargin: 5
        //        }
    }
}



