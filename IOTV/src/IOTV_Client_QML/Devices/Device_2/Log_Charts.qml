import QtQuick
import QtQuick.Controls
import QtCharts

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem

Page {
    //Ссылка на Device
    required property var device
    property list<bool> waitList

    id: root

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
        height: 30
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
        legend.visible: false
        margins.left: 10
        margins.right: 10
        margins.top: 0
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
            //            axisY: myAxisPressure
            color: "gray"
            width: 3
            axisYRight: myAxisPressure
        }

        MouseArea {
            anchors.fill: parent
            onWheel: (wheel)=> {
                         var angle = wheel.angleDelta.y
                         if (angle > 0)
                         chartView.zoomIn()
                         else
                         chartView.zoomOut()
                     }
            onMouseXChanged: {
                //                console.log(mouseX)
                //                chartView.scrollLeft(mouseX)
            }
        }

        Component.onCompleted: {
            myLegend.addSeries(lineSeriesTemperature, lineSeriesTemperature.name, lineSeriesTemperature.color)
            myLegend.addSeries(lineSeriesHumidity, lineSeriesHumidity.name, lineSeriesHumidity.color)
            myLegend.addSeries(lineSeriesPressure, lineSeriesPressure.name, lineSeriesPressure.color)
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
                obj = lineSeriesTemperature
            else if (channelNumber === 1)
                obj = lineSeriesHumidity
            else if (channelNumber === 2)
                obj = lineSeriesPressure

            if (device.logDataOverflow)
                glob_notification.set_text("Превышен размер данных!")
            else
            {
                // усконерние обавления точек!!!
                device.fillSeries(obj, points)
                //                for(var i = 0; i < points.length; i++)
                //                    obj.append(points[i].x, points[i].y)
            }

            waitList[channelNumber] = false

            for (var i = 0; i < waitList.length; i++)
            {
                if (waitList[i] === true)
                    return
            }

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

        waitList = [true, true, true]

        device.signalQueryLogData(dateStart, dateEnd, 100, 0, 0)
        device.signalQueryLogData(dateStart, dateEnd, 100, 1, 0)
        device.signalQueryLogData(dateStart, dateEnd, 100, 2, 0)

        busyIndicator.visible = true
    }
}
