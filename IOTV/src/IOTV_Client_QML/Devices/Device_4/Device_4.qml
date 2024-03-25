import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem

Page {
    id: root
    title: device.aliasName
    objectName: device.aliasName

    required property var device

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

                    highlighted: device.readData(0) === "true"

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

                        device.setDataFromString(0, "true")
                    }
                }

                BaseItem.AnimRoundButton {
                    id: stopButton

                    highlighted: device.readData(0) === "false"

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
                        device.setDataFromString(0, "false")
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

    Devices.BusyRect {
        id: popup
        anchors.fill: parent
        visible: !device.state
    }

    Connections {
        target: glob_deviceStackView
        function onHeightChanged() {
            item_eqDialog.y = mapFromItem(glob_deviceStackView, 0, 0).y + glob_deviceStackView.height / 2 - item_eqDialog.height / 2
        }
        function onWidthChanged() {
            item_eqDialog.y = mapFromItem(glob_deviceStackView, 0, 0).y + glob_deviceStackView.height / 2 - item_eqDialog.height / 2
        }
    }
    Connections {
        target: device
        function onSignalDataChanged(ch) {
            var res = device.readData(ch)
            if (ch === 0)
            {
                playButton.highlighted = res === "true"
                stopButton.highlighted = !playButton.highlighted
            }
            else if (ch === 1)
            {
                if (res === "1")
                {
                    playMode_1_Button.highlighted = true
                    playMode_2_Button.highlighted = false
                    playMode_3_Button.highlighted = false
                }
                else if (res === "2")
                {
                    playMode_2_Button.highlighted = true
                    playMode_1_Button.highlighted = false
                    playMode_3_Button.highlighted = false
                }
                else if (res === "3")
                {
                    playMode_3_Button.highlighted = true
                    playMode_1_Button.highlighted = false
                    playMode_2_Button.highlighted = false
                }
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

