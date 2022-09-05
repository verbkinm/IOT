import QtQuick 2.0
import QtQuick.Controls 2.15

Item {
    property alias name: base_device.name

    id: root
    width: 100
    height: 100

    Base_Device {
        id: base_device

//        onInfo: {
//            root.parent.showInfo(this.device)
//        }

//        device.onSignalDataReadRecived: {
//            var result = device.getReadChannelDataUI8(0) === 0 ? false : true
//            if(!result && switcher.buttonState)
//            {
//                switcher.checked = false
//                switcher.buttonState = false
//            }
//            else if(result && !switcher.buttonState)
//            {
//                switcher.checked = true
//                switcher.buttonState = true
//            }
//        }
    }

    property alias device: base_device.device // Должно стоять после Base_Device

    Item {
        id: main
        anchors.fill: parent
        enabled: false

        Switch {
            id: switcher
            property bool  buttonState: false
            anchors.centerIn: parent

            onToggled: {
                var value = checked ? 1 : 0
                base_device.device.writeDataUI8(0, value)
            }
        }
    }

    function setState(state)
    {
        main.enabled = state
    }
}
