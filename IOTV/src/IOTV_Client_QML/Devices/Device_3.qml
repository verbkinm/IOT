import QtQuick 2.0
import QtQuick.Controls 2.15

Item {
    property alias name: base_device.name

    id: root
    width: parent.width - 10
    height: 100

    Base_Device {
        id: base_device

        onInfo: {
            root.parent.showInfo(this.device)
        }

//        device.onSignalDataReadRecived: {
//            lbl.text = device.getReadChannelDataString(0)
//        }
    }

    property alias device: base_device.device // Должно стоять после Base_Device

    Item {
        id: main
        anchors.fill: parent
        enabled: false

        Label {
            id: lbl
            anchors.centerIn: parent
            text: "Данные не получены!"

//            onToggled: {
//                console.log(checked)
//            }
        }
    }

    function setState(state)
    {
        main.enabled = state
    }
}
