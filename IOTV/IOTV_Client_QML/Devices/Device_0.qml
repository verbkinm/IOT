import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2

Item {
    id: root
    width: 100 //parent.width - 10
    height: 100

    property alias name: base_device.name
    property alias device: base_device.device

    Base_Device {
        id: base_device
    }


    Item {
        id: main
        anchors.fill: parent
        enabled: false

        Image {
            id: img
            source: "qrc:/img/Unknow.png"
            sourceSize: Qt.size(64, 64)
            anchors.centerIn: parent
        }
    }
    function setState(state)
    {
        main.enabled = state
    }
}
