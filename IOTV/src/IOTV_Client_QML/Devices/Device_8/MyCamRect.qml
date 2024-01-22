import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtMultimedia

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem
import Producer


Item {
    //Ссылка на Device
    required property var device

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

        Producer{
            id: producer
            videoSink: videoOutput.videoSink
        }
        VideoOutput {
            id: videoOutput
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit

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

                image_origin: "qrc:/img/id_8/play.png"
                image_invert: "qrc:/img/id_8/play_white.png"

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

                image_origin: "qrc:/img/id_8/stop.png"
                image_invert: "qrc:/img/id_8/stop_white.png"

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
                enabled: false
                highlighted: false

                image_origin: "qrc:/img/id_8/volume.png"
                image_invert: "qrc:/img/id_8/volume_white.png"

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

            BaseItem.AnimRoundButton {
                id: rotateButton
                highlighted: false

                image_origin: "qrc:/img/id_8/repeate_on.png"
                image_invert: "qrc:/img/id_8/repeate_on_white.png"

                onClicked: {
                    videoOutput.orientation += 90
                }
            }

            BaseItem.AnimRoundButton {
                id: mirrorButton
                highlighted: false

                image_origin: "qrc:/img/id_8/mirror.png"
                image_invert: "qrc:/img/id_8/mirror_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        highlighted = false
                        producer.slotMirrored(false)
                    }
                    else
                    {
                        highlighted = true
                        producer.slotMirrored(true)
                    }
                }
            }
        }
    }

    Connections {
        target: device
        function onSignalDataPkgComplete(channel, data) {
            if (channel === 0)
            {
                var width = parseInt(device.readData(2), 10)
                var height = parseInt(device.readData(3), 10)
                producer.slotDataVideoFrame(width, height, data)
//                console.log("onSignalDataPkgComplete")
            }
            else if (channel === 1)
            {
                producer.slotDataAudioFrame(data);
            }
        }
    }

    Component.onCompleted: {
        console.log("MyCamRect construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("MyCamRect destruct: ", objectName)
    }
}
