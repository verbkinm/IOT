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

        device.onSignalDataReadRecived: {
            var result = device.getReadChannelDataUI8(0) === 0 ? false : true
            if(!result)
            {
                btn_img.source = "qrc:/img/led_off"
            }
            else if(result)
            {
                btn_img.source = "qrc:/img/led_on"
            }
        }
    }

    property alias device: base_device.device // Должно стоять после Base_Device

    Item {
        id: main
        anchors.fill: parent
        enabled: false

//        Switch {
//            id: switcher
//            property bool  buttonState: false
//            anchors.centerIn: parent

//            onToggled: {
//                var value = checked ? 1 : 0
//                base_device.device.writeDataUI8(0, value)
//            }
//        }
        Button {
            id: button
            anchors.centerIn: parent
            width: 60
            height: 60

            onClicked: {
                base_device.device.writeDataUI8(0, !device.getReadChannelDataUI8(0))
            }

            Image {
                id: btn_img
                source: "qrc:/img/led_off"
                sourceSize: Qt.size(38, 38)
                anchors.centerIn: parent
            }
        }
    }

    function setState(state)
    {
        main.enabled = state
    }
}
