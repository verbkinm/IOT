import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtMultimedia

import "qrc:/Devices/BaseItem" as BaseItem

Item {
    signal play()
    signal stop()
//    signal volumeOn()
    signal volumeOff()

    onVisibleChanged: {
        if (!visible)
        {
            stopButton.clicked()
            volumeOff()
        }
    }

    AudioOutput {

    }

    Rectangle {
        id: camRect
        width: parent.width
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        color: "black"

        Image {
            id: img
            cache: false

            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: "image://provider/frame"
            width: parent.width
            height: parent.height

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (playButton.highlighted)
                        stopButton.clicked()
                    else
                        playButton.clicked()
                }
            }
        }


    }

    Rectangle {
        anchors.top: camRect.bottom
        width: parent.width
        height: 80

        border.width: 1
        border.color: "black"

        Row {
            spacing: 5
            anchors.centerIn: parent

            BaseItem.AnimRoundButton {
                id: playButton

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
                    play()
//                    volumeOn()
                }
            }

            BaseItem.AnimRoundButton {
                id: stopButton

                highlighted: true

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
                    stop()
//                    volumeOff()
                }
            }

            BaseItem.AnimRoundButton {
                id: volumeButton

                highlighted: false

                image_origin: "qrc:/img/id_4/volume.png"
                image_invert: "qrc:/img/id_4/volume_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        highlighted = false
//                        volumeOn()
                    }
                    else
                    {
                        highlighted = true
//                        volumeOff()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("MyCamRect construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("MyCamRect destruct: ", objectName)
    }

    function reloadImage() {
        img.source = ""
        img.source = "image://provider/frame"
    }
}
