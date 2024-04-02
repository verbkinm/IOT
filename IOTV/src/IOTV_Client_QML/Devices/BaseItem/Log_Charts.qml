import QtQuick
import QtQuick.Controls
import QtCharts

import "qrc:/Devices/" as Devices

Page {
    //Ссылка на Device
    required property var device
    required property list<bool> waitList

    id: root

    TextField {
        id: txtDate
        height: 52
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        font.pixelSize: 14

        visible: global_window.inPortrait

        text: new Date().toLocaleDateString("d MM yyyy")
        readOnly: true
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 15
        }

        MouseArea {
            anchors.fill: parent
            onClicked: datePicker.open()
        }
    }

    DatePicker {
        id: datePicker
        width: parent.width - parent.width * 0.1
        selectedDate: new Date()

        onDateChanged: (date) =>{
                           txtDate.text = date.toLocaleDateString("d MM yyyy")

                           var dateStart = new Date(date)
                           dateStart.setHours(0)
                           dateStart.setMinutes(0)
                           dateStart.setSeconds(0)

                           var dateEnd = new Date(date)
                           dateEnd.setHours(23)
                           dateEnd.setMinutes(59)
                           dateEnd.setSeconds(59)

                           requestLogData(dateStart.getTime(), dateEnd.getTime(), 100)
                       }
    }

    ChartView {
        width: parent.width

        anchors.top: {
            if (txtDate.visible)
                return txtDate.bottom

            return parent.top
        }
        anchors.bottom: parent.bottom
        //        title: "XXX data read"
        antialiasing: true
        animationOptions: ChartView.SeriesAnimations
        titleFont.bold: true
        titleFont.pointSize: 15
        //        legend.visible:false
        margins.left: 10
        margins.right: 10
        margins.top: 10
        margins.bottom: 10


        // Время - x ось
        ValuesAxis {
            id: myAxisTime
            min: 0
            max: 24
            tickCount: 11
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelFormat: '%d'
        }

        // Температура - y ось
        ValuesAxis{
            id: myAxisTemperature
            min: -40
            max: 80
            tickCount: {
                if (global_window.inPortrait === true)
                    return 11
                return 7
            }
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelsColor: lineSeriesTemperature.color
            labelFormat: '%d'

            //            property real tmpMin: 1000
            //            property real tmpMax: -1000
        }

        // Влажность - y ось
        ValuesAxis {
            id: myAxisHumidity
            min: 0
            max: 100
            tickCount: myAxisTemperature.tickCount
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelsColor: lineSeriesHumidity.color
            labelFormat: '%d'
        }

        // Давление - y ось
        ValuesAxis {
            id: myAxisPressure
            min: 0
            max: 1100
            tickCount: myAxisHumidity.tickCount
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelsColor: lineSeriesPressure.color
            labelFormat: '%d'
        }

        LineSeries {
            onClicked: (point) =>{
                           console.log(point)
                       }

            id:lineSeriesTemperature
            name: "Температура ℃"
            axisX: myAxisTime
            axisY: myAxisTemperature
            color: "red"
            width: 3
        }

        LineSeries {
            id:lineSeriesHumidity
            name: "Влажность %"
            axisX: myAxisTime
            axisY: myAxisHumidity
            color: "blue"
            width: 3
        }

        LineSeries {
            id:lineSeriesPressure
            name: "Давление мм рт.ст."
            axisX: myAxisTime
            axisY: myAxisPressure
            color: "gray"
            width: 3
        }
    }

    Component.onCompleted: {
        console.log("Log_Cahrts construct")

        var dateStart = new Date
        dateStart.setHours(0)
        dateStart.setMinutes(0)
        dateStart.setSeconds(0)

        var dateEnd = new Date(dateStart)
        dateEnd.setHours(23)
        dateEnd.setMinutes(59)
        dateEnd.setSeconds(59)

        console.log(dateStart, dateEnd)

        requestLogData(dateStart.getTime(), dateEnd.getTime(), 100)
    }

    Component.onDestruction: {
        console.log("Log_Cahrts destruct")
    }

    Connections {
        target: device
        function onSignalResponceLogData(channelNumber) {

//            waitList[channelNumber] = false


//            for (var i = 0; i < 3; i++)
//            {
//                if (waitList[i] === true)
//                    return
//            }

//            console.log("all resived")

            //            if (channelNumber === 0)
            //                device.seriesAddData(lineSeriesTemperature, channelNumber, 0)
            //            else if (channelNumber === 1)
            //                device.seriesAddData(lineSeriesHumidity, channelNumber, 0)
            //            else if (channelNumber === 2)
            //                device.seriesAddData(lineSeriesPressure, channelNumber, 0)
        }
    }

    Devices.BusyRect {
        id: busyIndicator
        anchors.fill: parent
        visible: false
    }

    function yDataMin(axis, data, offset)
    {
        axis.min = data < axis.min ? data - offset : axis.min
    }

    function yDataMax(axis, data, offset)
    {
        axis.max = data > axis.max ? data + offset : axis.max
    }

    function requestLogData(dateStart, dateEnd, intervalMS) {
        device.signalQueryLogData(dateStart, dateEnd, intervalMS, 0, 0)
        device.signalQueryLogData(dateStart, dateEnd, intervalMS, 1, 0)
        device.signalQueryLogData(dateStart, dateEnd, intervalMS, 2, 0)

        busyIndicator.visible = true
    }
}
