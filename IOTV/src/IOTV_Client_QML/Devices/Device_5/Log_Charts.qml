import QtQuick
import QtQuick.Controls
import QtCharts

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem

Page {
    //Ссылка на Device
    required property var device
    property var waitList

    id: root

    RoundButton {
        id: previosDay

        highlighted: true
        width: height
        height: txtDate.height + 5

        icon {
            color: "transparent"
            source: "qrc:/img/back.png"
        }

        anchors {
            left: parent.left
            margins: 15
            verticalCenter: txtDate.verticalCenter
        }

        onClicked: {
            datePicker.selectedDate.setDate(datePicker.selectedDate.getDate() - 1)
        }
    }

    RoundButton {
        id: nextDay

        highlighted: true
        width: previosDay.width
        height: previosDay.height

        icon {
            color: "transparent"
            source: "qrc:/img/forward.png"
        }

        anchors {
            right: parent.right
            margins: previosDay.anchors.margins
            verticalCenter: txtDate.verticalCenter
        }

        onClicked: {
            datePicker.selectedDate.setDate(datePicker.selectedDate.getDate() + 1)
        }
    }

    TextField {
        id: txtDate
        height: 52
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        font.pixelSize: 14

        visible: global_window.inPortrait

        text: new Date().toLocaleDateString("dd MM yyyy")
        readOnly: true
        anchors {
            left: previosDay.right
            right: nextDay.left
            top: parent.top
            margins: 15
        }

        MouseArea {
            anchors.fill: parent
            onClicked: datePicker.open()
        }
    }

    BaseItem.DatePicker {
        id: datePicker
        width: parent.width - parent.width * 0.1
        selectedDate: new Date()

        onSelectedDateChanged: {
            txtDate.text = selectedDate.toLocaleDateString("dd MM yyyy")

            var dateStart = new Date(selectedDate)
            dateStart.setHours(0)
            dateStart.setMinutes(0)
            dateStart.setSeconds(0)

            var dateEnd = new Date(selectedDate)
            dateEnd.setHours(23)
            dateEnd.setMinutes(59)
            dateEnd.setSeconds(59)

            requestLogData(dateStart.getTime(), dateEnd.getTime())
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
        animationOptions: ChartView.AllAnimations
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

        // Реле - y ось
        ValuesAxis {
            id: myAxisRele
            min: 0
            max: 1
            tickCount: 2
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelsColor: lineSeriesRele.color
            labelFormat: '%d'
        }

        // Текущее расстояние - y ось
        ValuesAxis {
            id: myAxisDistacnce
            min: 0
            max: 255
            tickCount: myAxisHumidity.tickCount
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelsColor: lineSeriesDistance.color
            labelFormat: '%d'
        }

        LineSeries {
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

        LineSeries {
            id:lineSeriesRele
            name: "Реле"
            axisX: myAxisTime
            axisYRight: myAxisRele
            color: "green"
            width: 3
        }

        LineSeries {
            id:lineSeriesDistance
            name: "VL6180"
            axisX: myAxisTime
            axisYRight: myAxisDistacnce
            color: "orange"
            width: 3
        }
    }


    Component.onCompleted: {
        console.log("Log_Cahrts construct")
    }

    Component.onDestruction: {
        console.log("Log_Cahrts destruct")
    }

    Connections {
        target: device
        function onSignalResponceLogData(points, channelNumber, flags) {
            var obj;
            if (channelNumber === 0)
                obj = lineSeriesRele
            else if (channelNumber === 10)
                obj = lineSeriesDistance
            else if (channelNumber === 12)
                obj = lineSeriesTemperature
            else if (channelNumber === 13)
                obj = lineSeriesHumidity
            else if (channelNumber === 14)
                obj = lineSeriesPressure


            for(var i = 0; i < points.length; i++)
                obj.append(points[i].x, points[i].y)

            waitList[channelNumber] = false

            if (waitList[0] === true ||
                    waitList[10] === true ||
                    waitList[12] === true ||
                    waitList[13] === true ||
                    waitList[14] === true)
                return

            busyIndicator.visible = false
        }
    }

    Devices.BusyRect {
        id: busyIndicator
        anchors.fill: parent
        visible: false
    }

    function requestLogData(dateStart, dateEnd, intervalMS) {
        lineSeriesTemperature.clear()
        lineSeriesHumidity.clear()
        lineSeriesPressure.clear()
        lineSeriesRele.clear()
        lineSeriesDistance.clear()

        /*
        0 - реле
        10 - текущее расстояние
        12 - тепмпература
        13 - влажность
        14 - давление
          */
        waitList = []
        waitList[0] = true
        waitList[10] = true
        waitList[12] = true
        waitList[13] = true
        waitList[14] = true

        device.signalQueryLogData(dateStart, dateEnd, 100, 0, 0)
        device.signalQueryLogData(dateStart, dateEnd, 100, 10, 0)
        device.signalQueryLogData(dateStart, dateEnd, 60000, 12, 0)
        device.signalQueryLogData(dateStart, dateEnd, 60000, 13, 0)
        device.signalQueryLogData(dateStart, dateEnd, 60000, 14, 0)

        busyIndicator.visible = true
    }
}
