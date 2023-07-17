import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/" as Devices

Page {
    id: root
    title: device.aliasName
    objectName: device.aliasName

    required property var device

//    property bool play: false
//    property int playMode: 1
    property int ledMode: Number(device.readData(2))
//    property bool repeateMode: false
//    property int maxDuty: 8192
//    property int minDuty: 100
//    property int volume: 0
//    property int eq: 0

    readonly property string statePlay: "play"
    readonly property string stateStop: "stop"

    state: stateStop

    onStateChanged: {
        console.log(state)
    }

    states: [
        State {
            name: statePlay
            //            when: adc_value > adc_border && device.state
            //            PropertyChanges { target: play; icon.source: "qrc:/img/pause.png"}
            //            PropertyChanges { target: anim_led; running: true}
        },
        State {
            name: stateStop
            //            when: (adc_value <= adc_border) || !device.state
            //            PropertyChanges { target: play; icon.source: "qrc:/img/play.png"}
            //            PropertyChanges { target: anim_led; running: false}
            //            PropertyChanges { target: img_led; opacity: 0}
        }
    ]

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

            Image {
                id: img
                source: "qrc:/img/id_4/cloud_off.png"

                height: 200
                width: height

                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: {
                        //                    clickButton()
                        //                    device.setDataFromString(3, "true")
                    }
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 5

                RoundButton {
                    id: playButton
                    width: 72
                    height: 72

                    highlighted: device.readData(0) === "true"

                    Image {
                        anchors.centerIn: parent
                        source: playButton.highlighted ? "qrc:/img/id_4/play_white.png" : "qrc:/img/id_4/play.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        //                    clickButton()
                        //                        device.setDataFromString(3, "true")
                    }
                }

                RoundButton {
                    id: stopButton
                    width: 72
                    height: 72

                    highlighted: device.readData(0) === "false"

                    Image {
                        anchors.centerIn: parent
                        source: stopButton.highlighted ? "qrc:/img/id_4/stop_white.png" : "qrc:/img/id_4/stop.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        //                    clickButton()
                        //                        device.setDataFromString(3, "true")
                    }
                }

                RoundButton {
                    id: repeateButton
                    width: playButton.width
                    height: playButton.height

                    highlighted: device.readData(3) === "1"

                    Image {
                        anchors.centerIn: parent
                        source: repeateButton.highlighted ? "qrc:/img/id_4/repeate_on_white.png" : "qrc:/img/id_4/repeate_on.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit // ensure it fits
                    }

                    onClicked: {
                        //                        //                    clickButton()
                        //                        if (repeateMode)
                        //                            device.setDataFromString(1, "false")
                        //                        else
                        //                            device.setDataFromString(1, "true")
                    }
                }
            }
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 5

                RoundButton {
                    id: playMode_1_Button
                    width: playButton.width
                    height: playButton.height

                    highlighted: device.readData(1) === "1"

                    Image {
                        anchors.centerIn: parent
                        source: playMode_1_Button.highlighted ? "qrc:/img/id_4/note_white.png" : "qrc:/img/id_4/note.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit // ensure it fits
                    }

                    onClicked: {
                        //                    popup.open()
                        //                    popupTimer.start()
                        //                        device.setDataFromString(2, "0")
                    }
                }

                RoundButton {
                    id: playMode_2_Button
                    width: playButton.width
                    height: playButton.height

                    highlighted: device.readData(1) === "2"

                    Image {
                        anchors.centerIn: parent
                        source: playMode_2_Button.highlighted ? "qrc:/img/id_4/tree_white.png" : "qrc:/img/id_4/tree.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        //                    popup.open()
                        //                    popupTimer.start()
                        //                        device.setDataFromString(2, "1")
                    }
                }
                RoundButton {
                    id: playMode_3_Button
                    width: playButton.width
                    height: playButton.height

                    highlighted: device.readData(1) === "3"

                    Image {
                        anchors.centerIn: parent
                        source: playMode_3_Button.highlighted ? "qrc:/img/id_4/toy_white.png" : "qrc:/img/id_4/toy.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        //                    popup.open()
                        //                    popupTimer.start()
                        //                        device.setDataFromString(2, "2")
                    }
                }

                RoundButton {
                    id: ledModeButton
                    width: playButton.width
                    height: playButton.height

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/id_4/palette.png"
                        height: 22
                        width: 22
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        ledModeDialog.visible = true
                        //                        device.setDataFromString(2, "0")
                    }
                }
            }
        }
    }

    Dialog {
        id: ledModeDialog
        modal: true
        standardButtons: Dialog.Ok
//        horizontalPadding: 20
        leftMargin: 15
        rightMargin: 15

        width: appStack.width - leftMargin - rightMargin
        height: item_ledModeDialog.height + 70

        visible: false

        onVisibleChanged: {
            if (visible)
                y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledModeDialog.height
        }

        Item {
            id: item_ledModeDialog
            height: 70
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            Row {
                id: row_item_ledModeDialog
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                Repeater {
                    id: repeater
                    model: 7
                    RoundButton{
                        required property int index
                        checkable: true
                        text: index

                        highlighted: ledMode === index

                        onClicked: {
                            popup_item_ledModeDialog.open()
                            popup_item_ledModeDialog.waitForVal = -1
//                            highlighted = true
                            device.setDataFromString(2, index)
                        }
                    }
                }
            }

            Devices.BusyRect {
                property int waitForVal: ledMode
                id: popup_item_ledModeDialog
                anchors.fill: parent
                visible: waitForVal !== ledMode
            }
        }
    }

    Devices.BusyRect {
        id: popup
        anchors.fill: parent
        visible: !device.state
    }

    Connections {
        target: appStack
        function onHeightChanged() {
            ledModeDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledModeDialog.height
        }
        function onWidthChanged() {
            ledModeDialog.y = mapFromItem(appStack, 0, 0).y + appStack.height / 2 - ledModeDialog.height
        }
    }

    Connections {
        target: device
        function onSignalDataChanged(ch) {
            if (ch === 2) {
                ledMode = Number(device.readData(ch))
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

