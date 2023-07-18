import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem

Page {
    id: root
    title: device.aliasName
    objectName: device.aliasName

    required property var device

    //    property bool play: false
    //    property int playMode: 1
    //    property bool repeateMode: false
    //    property int maxDuty: 8192
    //    property int minDuty: 100
    //    property int volume: 0
    //    property int eq: 0

    //    readonly property string statePlay: "play"
    //    readonly property string stateStop: "stop"

    //    state: stateStop

    //    onStateChanged: {
    //        console.log(state)
    //    }

    //    states: [
    //        State {
    //            name: statePlay
    //            //            when: adc_value > adc_border && device.state
    //            //            PropertyChanges { target: play; icon.source: "qrc:/img/pause.png"}
    //            //            PropertyChanges { target: anim_led; running: true}
    //        },
    //        State {
    //            name: stateStop
    //            //            when: (adc_value <= adc_border) || !device.state
    //            //            PropertyChanges { target: play; icon.source: "qrc:/img/play.png"}
    //            //            PropertyChanges { target: anim_led; running: false}
    //            //            PropertyChanges { target: img_led; opacity: 0}
    //        }
    //    ]

    header: Devices.DeviceHeader {
        id: headerPanel
    }

    Flickable {
        id: fl
        enabled: device.state
        width: root.width
        height: root.height

        contentHeight: column.height + column.topPadding + column.spacing + overlayHeader.height

        ScrollBar.vertical: ScrollBar {
            id: scrollVer
            visible: active
        }

        ScrollBar.horizontal: ScrollBar {
            id: scrollHor
            visible: active
        }

        Column {
            id: column
            width: parent.width
            spacing: 15
            //            padding: 30

            MyImg {
                id: cloud_MyImg
                source: "qrc:/img/id_4/cloud_off.png"
                height: 250
                width: 250

                anchors.horizontalCenter: parent.horizontalCenter

                state: playButton.highlighted ? playing : stoped

                Connections {
                    target: playButton
                    function onHighlightedChanged() {
                        if (target.highlighted)
                        {
                            cloud_MyImg.state = cloud_MyImg.playing
                        }
                        else
                        {
                            cloud_MyImg.state = cloud_MyImg.stoped
                        }
                    }
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 5

                BaseItem.AnimRoundButton {
                    id: playButton

                    highlighted: device.readData(0) === "1"

                    image_origin: "qrc:/img/id_4/play.png"
                    image_invert: "qrc:/img/id_4/play_white.png"



                    onClicked: {
                        if (highlighted)
                        {
                            highlighted = false
                            highlighted = true
                        }
                        else
                            highlighted = true

                        stopButton.highlighted = false

                        device.setDataFromString(0, "1")
                    }
                }

                BaseItem.AnimRoundButton {
                    id: stopButton

                    highlighted: device.readData(0) === "0"

                    image_origin: "qrc:/img/id_4/stop.png"
                    image_invert: "qrc:/img/id_4/stop_white.png"

                    onClicked: {
                        if (highlighted)
                        {
                            highlighted = false
                            highlighted = true
                        }
                        else
                            highlighted = true

                        playButton.highlighted = false
                        device.setDataFromString(0, "0")
                    }
                }

                BaseItem.AnimRoundButton {
                    id: repeateButton

                    highlighted: device.readData(3) === "true"

                    image_origin: "qrc:/img/id_4/repeate_on.png"
                    image_invert: "qrc:/img/id_4/repeate_on_white.png"

                    onClicked: {
                        highlighted = !highlighted
                        device.setDataFromString(3, highlighted)
                    }
                }
            }
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 5

                BaseItem.AnimRoundButton {
                    id: playMode_1_Button

                    highlighted: device.readData(1) === "1"

                    image_origin: "qrc:/img/id_4/note.png"
                    image_invert: "qrc:/img/id_4/note_white.png"

                    onClicked: {
                        if (highlighted)
                        {
                            highlighted = false
                            highlighted = true
                        }
                        else
                            highlighted = true

                        device.setDataFromString(1, "1")
                        playMode_2_Button.highlighted = false
                        playMode_3_Button.highlighted = false
                    }
                }

                BaseItem.AnimRoundButton {
                    id: playMode_2_Button

                    highlighted: device.readData(1) === "2"

                    image_origin: "qrc:/img/id_4/tree.png"
                    image_invert: "qrc:/img/id_4/tree_white.png"

                    Image {
                        anchors.centerIn: parent
                        source: playMode_2_Button.highlighted ? "qrc:/img/id_4/tree_white.png" : "qrc:/img/id_4/tree.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        if (highlighted)
                        {
                            highlighted = false
                            highlighted = true
                        }
                        else
                            highlighted = true

                        device.setDataFromString(1, "2")
                        playMode_1_Button.highlighted = false
                        playMode_3_Button.highlighted = false
                    }
                }

                BaseItem.AnimRoundButton {
                    id: playMode_3_Button

                    highlighted: device.readData(1) === "3"

                    image_origin: "qrc:/img/id_4/toy.png"
                    image_invert: "qrc:/img/id_4/toy_white.png"

                    onClicked: {
                        if (highlighted)
                        {
                            highlighted = false
                            highlighted = true
                        }
                        else
                            highlighted = true

                        device.setDataFromString(1, "3")
                        playMode_1_Button.highlighted = false
                        playMode_2_Button.highlighted = false
                    }
                }

                BaseItem.AnimRoundButton {
                    id: ledModeButton

                    highlighted: device.readData(2) !== "0"

                    image_origin: "qrc:/img/id_4/palette.png"
                    image_invert: "qrc:/img/id_4/palette_white.png"

                    onClicked: {
                        highlighted = !highlighted
                        device.setDataFromString(2, highlighted ? "1" : "0")
                    }
                }
            }
        }
    }

    //    Dialog {
    //        id: ledModeDialog
    //        modal: true
    //        standardButtons: Dialog.Ok
    //        //        horizontalPadding: 20
    //        leftMargin: 15
    //        rightMargin: 15

    //        width: appStack.width - leftMargin - rightMargin
    //        height: item_ledModeDialog.height + 70

    //        visible: false

    //        onVisibleChanged: {
    //            if (visible)
    //                y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledModeDialog.height
    //        }

    //        Item {
    //            id: item_ledModeDialog
    //            height: 70
    //            anchors {
    //                left: parent.left
    //                right: parent.right
    //                top: parent.top
    //            }

    //            Row {
    //                id: row_item_ledModeDialog
    //                anchors.verticalCenter: parent.verticalCenter
    //                anchors.horizontalCenter: parent.horizontalCenter
    //                Repeater {
    //                    id: repeater
    //                    model: 7
    //                    RoundButton{
    //                        required property int index
    //                        checkable: true
    //                        text: index

    //                        highlighted: ledMode === index

    //                        onClicked: {
    //                            popup_item_ledModeDialog.open()
    //                            popup_item_ledModeDialog.waitForVal = -1
    //                            //                            highlighted = true
    //                            device.setDataFromString(2, index)
    //                        }
    //                    }
    //                }
    //            }

    //            Devices.BusyRect {
    //                property int waitForVal: ledMode
    //                id: popup_item_ledModeDialog
    //                anchors.fill: parent
    //                visible: waitForVal !== ledMode
    //            }
    //        }
    //    }

    Devices.BusyRect {
        id: popup
        anchors.fill: parent
        visible: !device.state
    }

    Connections {
//        target: playButton
//        function onHighlightedChanged()
//        {
//            if (target.highlighted)
//                cloud_MyImg.state = cloud_MyImg.playing
//            else
//                cloud_MyImg.state = cloud_MyImg.stoped
//        }

        //        target: appStack
        //        function onHeightChanged() {
        //            ledModeDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledModeDialog.height
        //        }
        //        function onWidthChanged() {
        //            ledModeDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledModeDialog.height
        //        }
    }

    Connections {
        target: device
        function onSignalDataChanged(ch) {
            var res = device.readData(ch)
            if (ch === 0)
            {
                playButton.highlighted = res === "1"
                if (playButton.highlighted)
                    stopButton.highlighted = false
                else
                    stopButton.highlighted = true
            }
            else if (ch === 1)
            {
                if (res === "1")
                    playMode_1_Button.clicked()
                else if (res === "2")
                    playMode_2_Button.clicked()
                else if (res === "3")
                    playMode_3_Button.clicked()
            }
            else if (ch === 2)
            {
                ledModeButton.highlighted = res !== "0"
            }
            else if (ch === 3)
            {
                repeateButton.highlighted = res === "true" ? true : false
            }
        }
    }

    Component.onCompleted: {
        console.log("Device 4 construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Device 4 destruct: ", objectName)
    }
}

