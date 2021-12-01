import QtQuick 2.15
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
            temperature.value = parseFloat(device.getReadChannelDataFloat(0)).toFixed(1)
            humidity.value = parseFloat(device.getReadChannelDataFloat(1)).toFixed(1)
            if(device.readChannelLength() === 3)
            {
                pressure.value = parseFloat(device.getReadChannelDataFloat(2)).toFixed(0)
                pressure.visible = true
            }
        }
    }

    property alias device: base_device.device // Должно стоять после Base_Device

    Item {
        id: main
        anchors.fill: parent
        enabled: false

        Row {
            anchors.centerIn: parent
            spacing: 5

            My_Arc {
                id: temperature
                width: 60
                height: 60
                minValue: -40
                maxValue: 80
                minMiddle: 15
                maxMiddle: 30
                postfix: "°C"
            }
            My_Arc {
                id: humidity
                width: 60
                height: 60
                minValue: 0
                maxValue: 100
                minMiddle: 35
                maxMiddle: 75
                postfix: "%"
            }
            My_Arc {
                id: pressure
                width: 60
                height: 60
                minValue: 0
                maxValue: 1000
                minMiddle: 500
                maxMiddle: 800
                postfix: "мм..."
                visible: false
            }
        }
    }

    function setState(state)
    {
        main.enabled = state
    }
}
