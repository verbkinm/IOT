import QtQuick 2.9
import QtQuick.Controls 2.2
//import QtMultimedia 5.9

Page {
    id: root
    title: device.aliasName
    objectName: device.aliasName

    required property var device
    property int adc_value: 0
    property bool repeateSate: false
    property string mode: "-1" //первый запуск popup закроется при изменении занчение mode
    property bool triggerState: false

    readonly property int adc_border: 50
    readonly property string stateActive: "active"
    readonly property string stateUnactive: "unactive"
    readonly property string stateOnline: "online"
    readonly property string stateOffline: "offline"

    state: stateOffline

    onStateChanged: {
        console.log(state)
    }

    states: [
        State {
            name: stateActive
            when: adc_value > adc_border
            PropertyChanges { target: img; source: "qrc:/img/cloud_on.png"}
            PropertyChanges { target: play; icon.source: "qrc:/img/pause.png"}
        },
        State {
            name: stateUnactive
            when: adc_value <= adc_border
            PropertyChanges { target: img; source: "qrc:/img/cloud_off.png"}
            PropertyChanges { target: play; icon.source: "qrc:/img/play.png"}
//            PropertyChanges { target: popup; visible: false}
        }/*,
        State {
            name: stateOnline
            when: device.state
            PropertyChanges { target: popup; visible: false}
        },
        State {
            name: stateOffline
            when: !device.state
            PropertyChanges { target: popup; visible: true}
        }*/
    ]

    header: DeviceHeader {
        id: headerPanel
    }

    Flickable {
        id: fl
        enabled: device.state
        width: root.width
        height: root.height

        ScrollBar.vertical: ScrollBar {
            id: scrollVer
            visible: active
        }

        ScrollBar.horizontal: ScrollBar {
            id: scrollHor
            visible: active
        }

        Image {
            id: img
            source: "qrc:/img/cloud_off.png"
            width: parent.width - (parent.width * 100 / 10)
            fillMode: Image.PreserveAspectFit

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            MouseArea {
                hoverEnabled: true
                anchors.fill: parent
                onClicked: {
//                    clickButton()
                    device.setDataFromString(3, "true")
                }
            }
        }

        Row {
            id: row
            anchors {
                top: img.bottom
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 5

            Button {
                id: play
                width: 52
                height: 52
                display: AbstractButton.IconOnly
                icon {
                    color: "transparent"
                    source: "qrc:/img/play.png"
                }
                font.pixelSize: 18

                onClicked: {
//                    clickButton()
                    device.setDataFromString(3, "true")
                }
            }

            Button {
                id: autorepeate
                width: play.width
                height: play.height
                display: AbstractButton.IconOnly
                icon {
                    color: "transparent"
                    source: repeateSate ? "qrc:/img/repeate_on.png" : "qrc:/img/repeate_off.png"
                }
                onClicked: {
//                    clickButton()
                    if (repeateSate)
                        device.setDataFromString(1, "false")
                    else
                        device.setDataFromString(1, "true")
                }
            }
            Button {
                id: mode1
                width: play.width
                height: play.height
                display: AbstractButton.IconOnly
                icon {
                    color: "transparent"
                    source: "qrc:/img/palette.png"
                }
                font.pixelSize: 18
                onClicked: {
//                    popup.open()
//                    popupTimer.start()
                    device.setDataFromString(2, "0")
                }
            }
            Button {
                id: mode2
                width: play.width
                height: play.height
                display: AbstractButton.IconOnly
                icon {
                    color: "transparent"
                    source: "qrc:/img/note.png"
                }
                onClicked: {
//                    popup.open()
//                    popupTimer.start()
                    device.setDataFromString(2, "1")
                }
            }
            Button {
                id: mode3
                width: play.width
                height: play.height
                display: AbstractButton.IconOnly
                icon {
                    color: "transparent"
                    source: "qrc:/img/tree.png"
                }
                onClicked: {
//                    popup.open()
//                    popupTimer.start()
                    device.setDataFromString(2, "2")
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Device 4 construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Device 4 destruct: ", objectName)
    }

    Timer {
        id: timer
        interval: 500
        repeat: true
        running: true
        onTriggered: {
//            if (adc > ADC_border || rS !== repeateSate)
//            {
//                popup.open()
//                popupTimer.start()
//            }

//            if (!device.state)
//            {
//                popupTimer.stop()
//                popup.open()
//            }

            adc_value = device.readData(0)
            repeateSate = device.readData(1) === "true" ? true : false
            mode = device.readData(2)
            triggerState = device.readData(3) === "true" ? true : false

            fl.contentHeight = img.height + row.height + headerPanel.height
        }
    }

//    Timer {
//        id: popupTimer
//        interval: 500
//        running: false
//        repeat: false
//        onTriggered: {
//            popup.close()
//        }
//    }



    //    MediaPlayer {
    //        id: player
    //        volume: 1.0
    //        audioRole: MediaPlayer.SonificationRole
    //        onError: {
    //            console.log("player error", player.errorString)
    //        }
    //    }

    BusyRect {
        id: popup
        visible: !device.state
    }

//    Connections {
//        target: device
//        function onStateChanged() {
//            if (!device.state)
//                popup.open()
//            else
//                popup.close()
//        }
//    }

    function clickButton()
    {
        //        player.source = "qrc:/audio/click.mp3"
        //        player.play()
//        popup.open()
    }
}

