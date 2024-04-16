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

    padding: 0

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
            right: zoomButton.left
            margins: previosDay.anchors.margins
            verticalCenter: txtDate.verticalCenter
        }

        onClicked: {
            datePicker.selectedDate.setDate(datePicker.selectedDate.getDate() + 1)
        }
    }

    RoundButton {
        id: zoomButton

        highlighted: true
        width: previosDay.width
        height: previosDay.height

        visible: global_window.inPortrait

        icon {
            color: "transparent"
            source: "qrc:/img/zoom_white.png"
        }

        anchors {
            right: parent.right
            margins: previosDay.anchors.margins
            verticalCenter: txtDate.verticalCenter
        }

        onClicked: {
            sliderZoomVertical.visible = !sliderZoomVertical.visible
            sliderZoomHorizont.visible = !sliderZoomHorizont.visible
        }
    }

    Slider {
        z: 1
        id: sliderZoomVertical
        orientation: Qt.Vertical
        from: -5.0
        to: 5.0
        value: 0
        visible: false
        stepSize: 0.1

        anchors {
            top: myLegend.bottom
            right: zoomButton.right
            bottom: sliderZoomHorizont.top
        }
        onValueChanged: {
            var sub = value - chartView.currentZoomVertical
            if (sub > 0)
            {
                var r = Qt.rect(chartView.plotArea.x, chartView.plotArea.y, chartView.plotArea.width, chartView.plotArea.height / (1 + sub));
                chartView.zoomIn(r)
            }
            else if (sub < 0)
            {
                r = Qt.rect(chartView.plotArea.x, chartView.plotArea.y, chartView.plotArea.width, chartView.plotArea.height * (1 - sub));
                chartView.zoomIn(r)
            }

            chartView.currentZoomVertical += sub
        }
    }

    Slider {
        z: 1
        id: sliderZoomHorizont
        orientation: Qt.Horizontal
        from: -5.0
        to: 5.0
        value: 0
        visible: false
        stepSize: 0.1

        anchors {
            left: parent.left
            right: sliderZoomVertical.left
            bottom: parent.bottom
        }

        onValueChanged: {
            var sub = value - chartView.currentZoomHorizontal
            if (sub > 0)
            {
                var r = Qt.rect(chartView.plotArea.x, chartView.plotArea.y, chartView.plotArea.width / (1 + sub), chartView.plotArea.height);
                chartView.zoomIn(r)
            }
            else if (sub < 0)
            {
                r = Qt.rect(chartView.plotArea.x, chartView.plotArea.y, chartView.plotArea.width * (1 - sub), chartView.plotArea.height);
                chartView.zoomIn(r)
            }

            chartView.currentZoomHorizontal += sub
        }
    }

    BaseItem.DatePicker {
        id: datePicker
        width: parent.width - parent.width * 0.1
        selectedDate: {
            var result = new Date()
            result.setHours(0, 0, 0)
            return result
        }

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
        property real currentZoomVertical: 0
        property real currentZoomHorizontal: 0

        id: chartView
        anchors {
            top: myLegend. bottom
            left: parent.left
            right: sliderZoomVertical.visible ? sliderZoomVertical.left : parent.right
            bottom: sliderZoomHorizont.visible ? sliderZoomHorizont.top : parent.bottom
        }
        antialiasing: false
        animationOptions: ChartView.NoAnimation
        legend.visible: false
        margins.left: 10
        margins.right: 10
        margins.top: 0
        margins.bottom: 10

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true

            property real xStart: 0
            property real yStart: 0

            onPressed: (mouse)=> {
                xStart = mouseX
                yStart = mouseY
                glob_swipeView.interactive = false
            }
            onReleased: {
                glob_swipeView.interactive = true
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
                myAxisTime.min = myAxisTime.defaultMin
                myAxisTime.max = myAxisTime.defaultMax

                myAxisTemperature.min = myAxisTemperature.defaultMin
                myAxisTemperature.max = myAxisTemperature.defaultMax

                myAxisHumidity.min = myAxisHumidity.defaultMin
                myAxisHumidity.max = myAxisHumidity.defaultMax

                myAxisPressure.min = myAxisPressure.defaultMin
                myAxisPressure.max = myAxisPressure.defaultMax

                chartView.currentZoomVertical = 0
                chartView.currentZoomHorizontal = 0

                sliderZoomVertical.value = 0.0
                sliderZoomHorizont.value = 0.0
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
            labelFormat: '%.1f'

            visible: lineSeriesTemperature.visible
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

            visible: lineSeriesHumidity.visible
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

            visible: lineSeriesPressure.visible
        }

        LineSeries {
            id:lineSeriesTemperature
            name: "Температура ℃"
            axisX: myAxisTime
            axisY: myAxisTemperature
            color: "red"
            width: 3

            onClicked: (point) =>{
                print(point)
            }
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
            //            axisY: myAxisPressure
            color: "gray"
            width: 3
            axisYRight: myAxisPressure
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
                // усконерние обновления точек!!!
                device.fillSeries(obj, points)
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

        waitList = [lineSeriesTemperature.visible, lineSeriesHumidity.visible, lineSeriesPressure.visible]

        if (lineSeriesTemperature.visible)
            device.signalQueryLogData(dateStart, dateEnd, 60000, 0, 0)
        if (lineSeriesHumidity.visible)
            device.signalQueryLogData(dateStart, dateEnd, 60000, 1, 0)
        if (lineSeriesPressure.visible)
            device.signalQueryLogData(dateStart, dateEnd, 60000 * 10, 2, 0)

        busyIndicator.visible = true
    }
}
