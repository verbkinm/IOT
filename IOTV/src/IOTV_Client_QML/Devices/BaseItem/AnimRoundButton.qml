import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem

RoundButton {
    id: root
    height: 72
    width: 72

    property string image_origin: ""
    property string image_invert: "" 

    signal pressAndHold()

    Timer {
        id: longPressTimer
        interval: 1000
        repeat: false
        running: false
        onTriggered: {
            root.pressAndHold()
        }
    }

    onPressedChanged: {
        if ( pressed ) {
            longPressTimer.running = true;
        } else {
            longPressTimer.running = false;
        }
    }

    Image {
        anchors.centerIn: parent
        source: root.highlighted ? image_invert : image_origin
        height: root.width * 0.5
        width: root.height * 0.5
        fillMode: Image.PreserveAspectFit
    }

    states: [
        State {
            name: "highlighted_true"
            when: root.highlighted
        },
        State {
            name: "highlighted_false"
            when: !root.highlighted
        }
    ]
    transitions: [
        Transition {
            to: "highlighted_true"
            ParallelAnimation {
                PropertyAnimation {
                    target: root
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 500
                }
                PropertyAnimation {
                    target: root
                    property: "scale"
                    easing.type: Easing.OutElastic
                    from: 0
                    to: 1
                    duration: 700
                }
            }

        },
        Transition {
            to: "highlighted_false"
            ParallelAnimation {
                PropertyAnimation {
                    target: root
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 500
                }
                PropertyAnimation {
                    target: root
                    property: "scale"
                    from: 0
                    to: 1
                    easing.type: Easing.OutElastic
                    duration: 700
                }
            }
        }
    ]
}
