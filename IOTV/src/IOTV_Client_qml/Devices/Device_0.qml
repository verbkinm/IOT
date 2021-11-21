import QtQuick 2.0

Item {
    id: root
    width: parent.width - 10
    height: 100

    Base_Device {
        id: base_device

        onInfo: {
            root.parent.showInfo(this.device)
        }
    }

    property alias name: base_device.name
    property alias device: base_device.device

    Item {
        id: main
        anchors.fill: parent
        enabled: false

        Image {
            id: img
            source: "qrc:/img/id_0"
            sourceSize: Qt.size(64, 64)
            anchors.centerIn: parent
        }
    }
    function setState(state)
    {
        main.enabled = state
    }
}
