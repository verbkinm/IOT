import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    id: root
    height: column.height + column.topPadding + column.spacing

    required property var device
    required property var channelTemperature
    required property var channelHumidity
    required property var channelPressure

//    anchors.fill: parent

    Column{
        id: column
        width: parent.width
        spacing: 15
//        topPadding: 30

        //Температура
        MeteoBlock_Item {
            id: temperature
            source: "qrc:/img/id_1/temperature.png"
            text: temperatureValue()

            onSignalClick:
                text = temperatureValue()
        }

        //Влажность
        MeteoBlock_Item {
            id: humidity
            source: "qrc:/img/id_1/humidity.png"
            text: humidityValue()

            onSignalClick:
                text = humidityValue()
        }

        //Давление
        MeteoBlock_Item {
            id: pressure
            source: "qrc:/img/id_1/pressure.png"
            text: pressureValue()
            visible: device.readChannelLength > 2

            onSignalClick:
                text = pressureValue()
        }

        onPositioningComplete: {
//            root.height = column.height + column.topPadding + column.spacing
        }
    }

    function temperatureValue()
    {
        var t = device.readData(channelTemperature)
        if (t > 0)
            t = "+" + t

        return t.slice(0, t.length - 2) + " °C"
    }

    function humidityValue()
    {
        var h = device.readData(channelHumidity)
        return h.slice(0, h.length - 2) + " %"
    }

    function pressureValue()
    {
        var p = Number(device.readData(channelPressure))
        return  p.toFixed(0) + " мм рт. ст."
    }

    Connections {
        target: device
        function onSignalDataChanged(channel) {
            if (channel === channelTemperature)
                temperature.text = temperatureValue()
            else if (channel === channelHumidity)
                humidity.text = humidityValue()
            else if (channel === channelPressure)
                pressure.text = pressureValue()
        }
    }

    Component.onCompleted: {
        console.log("Construct MeteoBlock")
        temperature.text = temperatureValue()
        humidity.text = humidityValue()
        pressure.text = pressureValue()
    }
    Component.onDestruction: {
        console.log("Destruct MeteoBlock")
    }
}
