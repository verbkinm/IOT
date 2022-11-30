import QtQuick 2.9
import QtQuick.Controls 2.2
//import QtMultimedia 5.9

Page {
    required property var device
    property bool playSate: false
    property bool ledSate: false
    property bool repeateSate: false
    property string mode: "0" //первый запуск popup закроется при изменении занчение mode

    id: root
    title: device.aliasName

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
            source: playSate ? "qrc:/img/cloud_on.png" : "qrc:/img/cloud_off.png"
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
                    clickButton()
                    device.setDataFromString(0, "true")
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
                    source: playSate ? "qrc:/img/pause.png" : "qrc:/img/play.png"
                }
                font.pixelSize: 18

                onClicked: {
                    clickButton()
                    device.setDataFromString(0, "true")
                }
            }
            Button {
                id: led
                width: play.width
                height: play.height
                display: AbstractButton.IconOnly
                icon {
                    color: "transparent"
                    source: ledSate ? "qrc:/img/lamp_on.png" : "qrc:/img/lamp_off.png"
                }
                onClicked: {
                    clickButton()
                    if (ledSate)
                        device.setDataFromString(1, "false")
                    else
                        device.setDataFromString(1, "true")
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
                    clickButton()
                    if (repeateSate)
                        device.setDataFromString(2, "false")
                    else
                        device.setDataFromString(2, "true")
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
                    popup.open()
                    popupTimer.start()
                    device.setDataFromString(3, "1")
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
                    popup.open()
                    popupTimer.start()
                    device.setDataFromString(3, "2")
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
                    popup.open()
                    popupTimer.start()
                    device.setDataFromString(3, "3")
                }
            }
        }
    }

    Component.onCompleted: {
        title = device.name
    }

    Timer {
        id: timer
        interval: 500
        repeat: true
        running: true
        onTriggered: {
            var pS = device.readData(0) === "true" ? true : false
            var lS = device.readData(1) === "true" ? true : false
            var rS = device.readData(2) === "true" ? true : false

            if (pS !== playSate || lS !== ledSate || rS !== repeateSate)
            {
                popup.open()
                popupTimer.start()
            }

            if (!device.state)
            {
                popupTimer.stop()
                popup.open()
            }

            playSate = pS
            ledSate = lS
            repeateSate = rS

            fl.contentHeight = img.height + row.height + headerPanel.height
        }
    }

    Timer {
        id: popupTimer
        interval: 500
        running: false
        repeat: false
        onTriggered: {
            popup.close()
        }
    }

    Component.onDestruction: {
        console.log("Device 4 destruct: ", objectName)
    }

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
    }

    Connections {
        target: device
        function onStateChanged() {
            if (!device.state)
                popup.open()
            else
                popup.close()
        }
    }

    function clickButton()
    {
        //        player.source = "qrc:/audio/click.mp3"
        //        player.play()
        popup.open()
    }
}

