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

    objectName: "Log Chart id 5"

    RoundButton {
        id: previosDay

        highlighted: true
        width: height
        height: txtDate.height + 5

        visible: global_window.inPortrait

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

        visible: global_window.inPortrait

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

    BaseItem.MyLegend {
        id: myLegend
        width: parent.width
        anchors.top: {
            if (txtDate.visible)
                return txtDate.bottom

            return parent.top
        }
    }

    ChartView {
        id: chartView
        width: parent.width

        anchors.top: myLegend. bottom
        anchors.bottom: parent.bottom
        //        title: "XXX data read"
        antialiasing: true
        animationOptions: ChartView.NoAnimation
        titleFont.bold: true
        titleFont.pointSize: 15
        legend.visible:false
        margins.left: 10
        margins.right: 10
        margins.top: 10
        margins.bottom: 10

        MouseArea {
            acceptedButtons: Qt.RightButton
            anchors.fill: parent

            property real xStart: 0
            property real yStart: 0

            onPressed: {
                xStart = mouseX
                yStart = mouseY
            }

            onPositionChanged: {
                let dx = xStart - mouseX
                let dy = yStart - mouseY

                if (dx < 0)
                    chartView.scrollLeft(dx * -1)
                else
                    chartView.scrollRight(dx)

                if (dy < 0)
                    chartView.scrollUp(dy * -1)
                else
                    chartView.scrollDown(dy)

                xStart = mouseX
                yStart = mouseY
            }

            onDoubleClicked: {
                chartView.zoomReset()

                myAxisTime.min = myAxisTime.defaultMin
                myAxisTime.max = myAxisTime.defaultMax

                myAxisTemperature.min = myAxisTemperature.defaultMin
                myAxisTemperature.max = myAxisTemperature.defaultMax

                myAxisHumidity.min = myAxisHumidity.defaultMin
                myAxisHumidity.max = myAxisHumidity.defaultMax

                myAxisPressure.min = myAxisPressure.defaultMin
                myAxisPressure.max = myAxisPressure.defaultMax

                myAxisRele.min = myAxisRele.defaultMin
                myAxisRele.max = myAxisRele.defaultMax

                myAxisDistacnce.min = myAxisDistacnce.defaultMin
                myAxisDistacnce.max = myAxisDistacnce.defaultMax
            }
            onWheel: (wheel)=> {
                         var angle = wheel.angleDelta.y
                         if (angle > 0)
                             chartView.zoom(1.1)
                         else
                             chartView.zoom(0.9)
                     }
        }

        // Время - x ось
        DateTimeAxis {
            property date defaultMin
            property date defaultMax

            id: myAxisTime
            min: defaultMin
            max: defaultMax

            tickCount: 5
            labelsFont.pointSize: 8
            labelsFont.bold: true
            format: global_window.inPortrait ? "hh:mm" :"hh:mm:ss"

            defaultMin: {
                var result = new Date(datePicker.selectedDate)
                result.setHours(0, 0, 0)
                return result
            }
            defaultMax: {
                var result = new Date(datePicker.selectedDate)
                result.setHours(0, 0, 0)
                result.setDate(result.getDate() + 1)
                return result
            }
        }

        // Температура - y ось
        ValuesAxis{
            readonly property int defaultMin: -40
            readonly property int defaultMax: 80

            id: myAxisTemperature
            min: defaultMin
            max: defaultMax
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
            readonly property int defaultMin: 0
            readonly property int defaultMax: 100

            id: myAxisHumidity
            min: defaultMin
            max: defaultMax
            tickCount: myAxisTemperature.tickCount
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelsColor: lineSeriesHumidity.color
            labelFormat: '%d'
        }

        // Давление - y ось
        ValuesAxis {
            readonly property int defaultMin: 0
            readonly property int defaultMax: 1100

            id: myAxisPressure
            min: defaultMin
            max: defaultMax
            tickCount: myAxisHumidity.tickCount
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelsColor: lineSeriesPressure.color
            labelFormat: '%d'
        }

        // Реле - y ось
        ValuesAxis {
            readonly property int defaultMin: 0
            readonly property int defaultMax: 1

            id: myAxisRele
            min: defaultMin
            max: defaultMax
            tickCount: 2
            labelsFont.pointSize: 8
            labelsFont.bold: true
            labelsColor: lineSeriesRele.color
            labelFormat: '%d'
        }

        // Текущее расстояние - y ось
        ValuesAxis {
            readonly property int defaultMin: 0
            readonly property int defaultMax: 255

            id: myAxisDistacnce
            min: defaultMin
            max: defaultMax
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
            name: "Давление мм..."
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

        Component.onCompleted: {
            myLegend.addSeries(lineSeriesTemperature, lineSeriesTemperature.name, lineSeriesTemperature.color)
            myLegend.addSeries(lineSeriesHumidity, lineSeriesHumidity.name, lineSeriesHumidity.color)
            myLegend.addSeries(lineSeriesPressure, lineSeriesPressure.name, lineSeriesPressure.color)
            myLegend.addSeries(lineSeriesRele, lineSeriesRele.name, lineSeriesRele.color)
            myLegend.addSeries(lineSeriesDistance, lineSeriesDistance.name, lineSeriesDistance.color)
        }
    }


    Component.onCompleted: {
        console.log("Log_Cahrts construct")
    }

    Component.onDestruction: {
        console.log("Log_Cahrts destruct")
    }

    Connections {
        target: myLegend
        function onSelected(series){
            series.visible = !series.visible
        }
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

            if (device.logDataOverflow)
                glob_notification.set_text("Превышен размер данных!")
            else
                device.fillSeries(obj, points)

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

        device.signalQueryLogData(dateStart, dateEnd, 60000, 0, 0)
        device.signalQueryLogData(dateStart, dateEnd, 60000, 10, 0)
        device.signalQueryLogData(dateStart, dateEnd, 60000 * 10, 12, 0)
        device.signalQueryLogData(dateStart, dateEnd, 60000 * 10, 13, 0)
        device.signalQueryLogData(dateStart, dateEnd, 60000 * 30, 14, 0)

        busyIndicator.visible = true
    }
}
