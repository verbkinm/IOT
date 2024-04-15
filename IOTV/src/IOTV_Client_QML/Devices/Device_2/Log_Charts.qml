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

//                MouseArea {
//        //            acceptedButtons: Qt.RightButton
//                    anchors.fill: parent

//                    property real xStart: 0
//                    property real yStart: 0

//                    onPressed: {
//                        xStart = mouseX
//                        yStart = mouseY
//                    }

//                    onPositionChanged: {
//                        let dx = xStart - mouseX
//                        let dy = yStart - mouseY

//                        if (dx < 0)
//                            chartView.scrollLeft(dx * -1)
//                        else
//                            chartView.scrollRight(dx)

//                        if (dy < 0)
//                            chartView.scrollUp(dy * -1)
//                        else
//                            chartView.scrollDown(dy)

//                        xStart = mouseX
//                        yStart = mouseY
//                    }

//                    onDoubleClicked: {
//                        chartView.zoomReset()

//                        myAxisTime.min = myAxisTime.defaultMin
//                        myAxisTime.max = myAxisTime.defaultMax

//                        myAxisTemperature.min = myAxisTemperature.defaultMin
//                        myAxisTemperature.max = myAxisTemperature.defaultMax

//                        myAxisHumidity.min = myAxisHumidity.defaultMin
//                        myAxisHumidity.max = myAxisHumidity.defaultMax

//                        myAxisPressure.min = myAxisPressure.defaultMin
//                        myAxisPressure.max = myAxisPressure.defaultMax
//                    }
//                    onWheel: (wheel)=> {
//                                 var angle = wheel.angleDelta.y
//                                 if (angle > 0)
//                                 chartView.zoom(1.1)
//                                 else
//                                 chartView.zoom(0.9)
//                             }
//                }

        //???
//        TapHandler {
//            acceptedDevices: PointerDevice.TouchScreen | PointerDevice.TouchPad | PointerDevice.Mouse | PointerDevice.Stylus | PointerDevice.Airbrush | PointerDevice.Puck
//            onLongPressed: {
//                chartView.zoomReset()

//                myAxisTime.min = myAxisTime.defaultMin
//                myAxisTime.max = myAxisTime.defaultMax

//                myAxisTemperature.min = myAxisTemperature.defaultMin
//                myAxisTemperature.max = myAxisTemperature.defaultMax

//                myAxisHumidity.min = myAxisHumidity.defaultMin
//                myAxisHumidity.max = myAxisHumidity.defaultMax

//                myAxisPressure.min = myAxisPressure.defaultMin
//                myAxisPressure.max = myAxisPressure.defaultMax

//                print("onLongPressed")
//            }
//        }

        PinchArea{
            id: pa
            anchors.fill: parent
            property real currentPinchScaleX: 1
            property real currentPinchScaleY: 1
            property real pinchStartX : 0
            property real pinchStartY : 0

            onPinchStarted: (pinch) =>{
                                // Pinching has started. Record the initial center of the pinch
                                // so relative motions can be reversed in the pinchUpdated signal
                                // handler
                                pinchStartX = pinch.center.x;
                                pinchStartY = pinch.center.y;

                                print("onPinchStarted")
                            }

            onPinchUpdated: (pinch) => {
                                chartView.zoomReset();

                                // Reverse pinch center motion direction
                                var center_x = pinchStartX + (pinchStartX - pinch.center.x);
                                var center_y = pinchStartY + (pinchStartY - pinch.center.y);

                                // Compound pinch.scale with prior pinch scale level and apply
                                // scale in the absolute direction of the pinch gesture
                                var scaleX = currentPinchScaleX * (1 + (pinch.scale - 1) * Math.abs(Math.cos(pinch.angle * Math.PI / 180)));
                                var scaleY = currentPinchScaleY * (1 + (pinch.scale - 1) * Math.abs(Math.sin(pinch.angle * Math.PI / 180)));

                                // Apply scale to zoom levels according to pinch angle
                                var width_zoom = height / scaleX;
                                var height_zoom = width / scaleY;

                                var r = Qt.rect(center_x - width_zoom / 2, center_y - height_zoom / 2, width_zoom, height_zoom);
                                chartView.zoomIn(r);
                            }

            onPinchFinished: (pinch) => {
                                 // Pinch finished. Record compounded pinch scale.
                                 currentPinchScaleX = currentPinchScaleX * (1 + (pinch.scale - 1) * Math.abs(Math.cos(pinch.angle * Math.PI / 180)));
                                 currentPinchScaleY = currentPinchScaleY * (1 + (pinch.scale - 1) * Math.abs(Math.sin(pinch.angle * Math.PI / 180)));
                             }
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
                }
                onWheel: (wheel)=> {
                             var angle = wheel.angleDelta.y
                             if (angle > 0)
                             chartView.zoom(1.1)
                             else
                             chartView.zoom(0.9)
                         }
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

        LineSeries {
            //            onClicked: (point) =>{
            //                           console.log(point)
            //                       }

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
                print(channelNumber, points.length)
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
