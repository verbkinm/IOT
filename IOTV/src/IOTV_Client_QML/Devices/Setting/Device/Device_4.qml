import QtQuick 2.9
import QtQuick.Controls 2.15

import "qrc:/Devices/BaseItem" as BaseItem

Rectangle {
    //Ссылка на Device
    required property var device

    id: componentRect
    width: parent.width
    height: 180

    color: Qt.rgba(0, 0, 0, 0)

    Column {
        width: parent.width

        spacing: 15

        leftPadding: 15
        rightPadding: 15
        topPadding: 100

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            //            width: parent.width

            BaseItem.AnimRoundButton {
                id: volumeButton

                highlighted: true

                image_origin: "qrc:/img/id_4/volume.png"
                image_invert: "qrc:/img/id_4/volume_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        highlighted = false
                        highlighted = true
                    }
                    else
                        highlighted = true

                    volumeDialog.open()
                }
            }
            BaseItem.AnimRoundButton {
                id: eqButton

                highlighted: true

                image_origin: "qrc:/img/id_4/equalizer.png"
                image_invert: "qrc:/img/id_4/equalizer_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        highlighted = false
                        highlighted = true
                    }
                    else
                        highlighted = true

                    eqDialog.open()
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            BaseItem.AnimRoundButton {
                id: dutyMaxButton

                highlighted: true

                image_origin: "qrc:/img/id_4/lamp.png"
                image_invert: "qrc:/img/id_4/lamp_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        highlighted = false
                        highlighted = true
                    }
                    else
                        highlighted = true

                    dumyDialog.open()
                }
            }

            BaseItem.AnimRoundButton {
                id: ledButton

                highlighted: true

                image_origin: "qrc:/img/id_4/led.png"
                image_invert: "qrc:/img/id_4/led_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        highlighted = false
                        highlighted = true
                    }
                    else
                        highlighted = true

                    ledManualDialog.open()
                }
            }

            BaseItem.AnimRoundButton {
                id: ledModeButton

                highlighted: true

                image_origin: "qrc:/img/id_4/palette.png"
                image_invert: "qrc:/img/id_4/palette_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        highlighted = false
                        highlighted = true
                    }
                    else
                        highlighted = true

                    ledModeDialog.open()
                }
            }
        }
    }

    Dialog {
        id: volumeDialog
        modal: true
        title: "Громкость"
        leftMargin: 15
        rightMargin: 15

        width: appStack.width - leftMargin - rightMargin
        height: item_volumeDialog.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
                y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - volumeDialog.height / 2
        }



        Item {
            id: item_volumeDialog
            height: 300
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            Row {
                id: row_item_volumeDialog
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 45

                Label {
                    text: addZero(parseInt(sliderVolume.value), 2)
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 64
                }

                Slider {
                    id: sliderVolume
                    from: 0
                    to: 30

                    height: 140
                    scale: 2

                    orientation: Qt.Vertical
                    value: parseFloat(device.readData(6))

                    onValueChanged: {
                        device.setDataFromString(6, parseInt(value).toString())
                    }

                    onVisibleChanged: {
                        if (visible)
                            value = parseFloat(device.readData(6))
                    }
                }
            }
        }
    }

    Dialog {
        id: eqDialog
        modal: true
        title: "Эквалайзер"
        leftMargin: 15
        rightMargin: 15

        width: appStack.width - leftMargin - rightMargin
        height: item_eqDialog.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
                y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - eqDialog.height / 2
        }

        Item {
            id: item_eqDialog
            height: 320
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            Column {
                id: row_item_eqModeDialog
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: ["норма", "поп", "рок", "джаз", "классика", "басы"]

                    RadioButton {
                        required property int index
                        required property string modelData

                        checked: parseInt(device.readData(8)) === index
                        text: modelData

                        onClicked: {
                            device.setDataFromString(8, index.toString())
                        }

                        onVisibleChanged: {
                            checked = parseInt(device.readData(8)) === index
                        }


                        //                            Connections {
                        //                                target: device
                        //                                function onSignalDataChanged(channel) {
                        //                                    if (channel === 8)
                        //                                    {
                        //                                        checked = parseInt(device.readData(8)) === index
                        //                                    }
                        //                                }
                        //                            }
                    }
                }
            }

        }
    }

    Dialog {
        id: dumyDialog
        modal: true
        title: "Яркость"
        leftMargin: 15
        rightMargin: 15

        width: appStack.width - leftMargin - rightMargin
        height: item_dutyMinDialog.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
                y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - dumyDialog.height / 2
        }


        Item {
            id: item_dutyMinDialog
            height: 240

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            Row {
                id: row_item_dutyMinDialog
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 45

                Column {
                    spacing: 30

                    Row {
                        spacing: 15
                        Label {
                            text: "Max -"
                            font.pixelSize: 32
                        }

                        Label {
                            text: addZero(parseInt(rangeSliderDuty.second.value), 4)
                            font.pixelSize: 32
                        }
                    }
                    Row {
                        spacing: 15
                        Label {
                            text: "Min - "
                            font.pixelSize: 32
                        }

                        Label {
                            text: addZero(parseInt(rangeSliderDuty.first.value), 4)
                            font.pixelSize: 32
                        }
                    }
                }

                RangeSlider {
                    id: rangeSliderDuty
                    orientation: Qt.Vertical

                    from: 0
                    to: 8192

                    height: 120
                    scale: 2

                    first.value: parseFloat(device.readData(5))
                    second.value: parseFloat(device.readData(4))

                    first.onMoved: {
                        device.setDataFromString(5, parseInt(first.value))
                    }

                    second.onMoved:  {
                        device.setDataFromString(4, parseInt(second.value))
                    }

                    onVisibleChanged: {
                        if (visible)
                        {
                            first.value = parseFloat(device.readData(5))
                            second.value = parseFloat(device.readData(4))
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: ledModeDialog
        modal: true
        title: "Режим света"
        leftMargin: 15
        rightMargin: 15

        width: appStack.width - leftMargin - rightMargin
        height: item_ledDialog.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
                y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledModeDialog.height / 2
        }

        Item {
            id: item_ledDialog
            height: 320
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            Column {
                id: row_item_ledModeDialog
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: 6

                    RadioButton {
                        required property int index

                        checked: parseInt(device.readData(2)) === index
                        text: index

                        onClicked: {
                            device.setDataFromString(2, index)
                        }

                        onVisibleChanged: {
                            checked = parseInt(device.readData(2)) === index
                        }
                    }
                }
            }

        }
    }

    Dialog {
        id: ledManualDialog
        modal: true
        title: "Управление светодиодами"
        leftMargin: 15
        rightMargin: 15

        width: appStack.width - leftMargin - rightMargin
        height: item_ledManualDialog.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
                y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledManualDialog.height / 2
        }

        Item {
            id: item_ledManualDialog
            height: 280
            width: parent.width
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            Rectangle {
                id: ledNumberRect
                anchors.fill: parent
                color: Qt.rgba(0, 0, 0, 0.0)


                ComboBox {
                    id: comboBoxLedManual
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        topMargin: 25
                    }

                    model: ["Светодиод №1", "Светодиод №2", "Светодиод №3", "Светодиод №4", "Светодиод №5"]

                    onCurrentIndexChanged: {
                        checkBoxLedManualRed.checked = false
                        checkBoxLedManualGreen.checked = false
                        checkBoxLedManualBlue.checked = false
                    }
                }

                Rectangle {
                    height: 120

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: comboBoxLedManual.bottom
                        topMargin: 25
                    }

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter

                        spacing: 10

                        Rectangle {
                            width: 100
                            height: 100
                            color: "red"
                            radius: width / 2

                            MouseArea {
                                anchors.fill: parent
                                onClicked:
                                {
                                    checkBoxLedManualRed.checked = !checkBoxLedManualRed.checked
                                    ledManual()
                                }
                            }

                            CheckBox {
                                id: checkBoxLedManualRed
                                anchors.centerIn: parent
                                onToggled: ledManual()
                            }

                        }

                        Rectangle {
                            width: 100
                            height: 100
                            color: "green"
                            radius: width / 2

                            MouseArea {
                                anchors.fill: parent
                                onClicked:
                                {
                                    checkBoxLedManualGreen.toggle()
                                    ledManual()
                                }
                            }

                            CheckBox {
                                id: checkBoxLedManualGreen
                                anchors.centerIn: parent
                                onToggled: ledManual()
                            }
                        }

                        Rectangle {
                            width: 100
                            height: 100
                            color: "blue"
                            radius: width / 2

                            MouseArea {
                                anchors.fill: parent
                                onClicked:
                                {
                                    checkBoxLedManualBlue.toggle()
                                    ledManual()
                                }
                            }

                            CheckBox {
                                id: checkBoxLedManualBlue
                                anchors.centerIn: parent
                                onToggled: ledManual()
                            }
                        }
                    }
                }
            }
        }
    }


    Connections {
        target: appStack
        function onHeightChanged() {
            volumeDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - volumeDialog.height / 2
            eqDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - eqDialog.height / 2
            dumyDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - dumyDialog.height / 2
            ledManualDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledManualDialog.height / 2
        }
        function onWidthChanged() {
            volumeDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - volumeDialog.height / 2
            eqDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - eqDialog.height / 2
            dumyDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - dumyDialog.height / 2
            ledManualDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledManualDialog.height / 2
        }
    }

    Connections {
        target: device
        function onSignalDataChanged(channel)
        {
            var val = device.readData(channel)

            //            if (channel === 4)
            //            {
            //                rangeSliderDuty.second.value = val
            //            }
            //            else if (channel === 5)
            //                rangeSliderDuty.first.value = val
            //            else if (channel === 6)
            //                sliderVolume.value = val
            //            else if (channel === 7)
            ////                dateTime.year = Number(device.readData(8))
        }
    }

    Component.onCompleted: {
        console.log("Device settings 5 construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Device settings 5 destruct: ", objectName)
    }

    //    function getRange(min, max) {
    //        var arr = []
    //        var j = 0
    //        for (var i = min; i <= max; ++i) {
    //            arr[j] = i
    //            ++j
    //        }

    //        return arr
    //    }

    function addZero(val, stringWidth) {
        var result = ""
        result = val.toString()

        while (result.length < stringWidth)
            result = "0" + result;

        return result
    }

    function ledManual()
    {
        const LED_RED = 0
        const LED_GREEN = 1
        const LED_BLUE = 2
        const LED_YELLOW = 3
        const LED_LIGHTBLUE = 4
        const LED_MAGENTA = 5
        const LED_WHITE = 6
        const LED_NONE = 7

        var color = LED_NONE
        var ledNumder = comboBoxLedManual.currentIndex
        var red = checkBoxLedManualRed.checked
        var green = checkBoxLedManualGreen.checked
        var blue = checkBoxLedManualBlue.checked

        if (red && !green && !blue)
            color = LED_RED;
        else if (!red && green && !blue)
            color = LED_GREEN;
        else if (!red && !green && blue)
            color = LED_BLUE;
        else if (red && green && !blue)
            color = LED_YELLOW;
        else if (!red && green && blue)
            color = LED_LIGHTBLUE;
        else if (red && !green && blue)
            color = LED_MAGENTA;
        else if (red && green && blue)
            color = LED_WHITE;

        var resultData = ledNumder.toString() + color.toString();

        console.log(resultData)
        device.setDataFromString(7, resultData)
    }
}
