import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtMultimedia
import QtCore

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

    id: root

    onVisibleChanged: {
        if (!visible)
        {
            disableStream()
        }
    }

    //    AudioOutput {

    //    }

    Settings {
        id: setting
        category: device.aliasName
        property alias orientation: videoOutput.orientation
        property bool mirror
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
            video_device: root.device
//            audioSink: audioOutput.device
        }

//        AudioOutput {
//            id: audioOutput
//            volume: 1
//        }

        VideoOutput {
            id: videoOutput
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            orientation: setting.orientation

            width: parent.width
            height: parent.height

            MouseArea {
                anchors.fill: parent
                onClicked: {
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
                height: 64
                width: 64

                image_origin: "qrc:/img/id_8/stop.png"
                image_invert: "qrc:/img/id_8/play_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        stop()
                        highlighted = false
                    }
                    else
                    {
                        play()
                        highlighted = true
                    }
                }
            }

            BaseItem.AnimRoundButton {
                id: volumeButton
                height: 64
                width: 64

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
                height: 64
                width: 64

                highlighted: false

                image_origin: "qrc:/img/id_8/repeate_on.png"
                image_invert: "qrc:/img/id_8/repeate_on_white.png"

                onClicked: {
                    videoOutput.orientation = (videoOutput.orientation + 90) % 360
                }
            }

            BaseItem.AnimRoundButton {
                id: mirrorButton

                height: 64
                width: 64

                highlighted: false

                image_origin: "qrc:/img/id_8/mirror.png"
                image_invert: "qrc:/img/id_8/mirror_white.png"

                onClicked: {
                    if (highlighted)
                    {
                        setting.mirror = false
                        highlighted = false
                        producer.slotMirrored(false)
                    }
                    else
                    {
                        setting.mirror = true
                        highlighted = true
                        producer.slotMirrored(true)
                    }
                }

                Component.onCompleted: {
                    if (setting.mirror)
                        mirrorButton.clicked()
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("MyCamRect construct: ", objectName)
    }

    Component.onDestruction: {
        disableStream()
        console.log("MyCamRect destruct: ", objectName)
    }

    function disableStream()
    {
        playButton.highlighted = false
        stop()
        volumeOff()
    }
}
