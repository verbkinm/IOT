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
                    id: row_item_ledModeDialog
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
                    //                    spacing: 15

                    Repeater {
                        model: ["норма", "поп", "рок", "джаз", "классика", "басы"]

                        RadioButton {
                            required property int index
                            required property string modelData

                            checked: parseInt(device.readData(0)) === index
                            text: modelData

                            Connections {
                                target: device
                                function onSignalDataChanged(channel) {
                                    if (channel === 0)
                                    {
                                        checked = parseInt(device.readData(0)) === index
                                    }
                                }
                            }
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
        }
        function onWidthChanged() {
            volumeDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - volumeDialog.height / 2
            eqDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - eqDialog.height / 2
            dumyDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - dumyDialog.height / 2
        }
    }

    Connections {
        target: device
        function onSignalDataChanged(channel)
        {
            var val = device.readData(channel)

            if (channel === 4)
                rangeSliderDuty.first.value = val
            else if (channel === 5)
                rangeSliderDuty.second.value = val
            else if (channel === 6)
                sliderVolume.value = val
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
}
