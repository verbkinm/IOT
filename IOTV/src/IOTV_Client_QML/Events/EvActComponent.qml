import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Effects

Item {
    readonly property string statePressed: "pressed"
    readonly property string stateRealesed: "released"

    signal signalClick()

    id: root

    width: global_window.width / 2 - global_window.width * 0.1
    height: 110

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: signalClick()
    }

    Rectangle {
        id: componentRect
        anchors.centerIn: parent

        width: root.width - 15
        height: 100

        color: Qt.rgba(255, 255, 255, 1)
        radius: 5

        Image {
            id: icon
            source: model.source
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
                topMargin: 10
            }
            width: 54
            height: 54
            fillMode: Image.PreserveAspectFit
        }

        Label {
            id: evName
            text: model.name
            font.pixelSize: 16

            anchors {
                left: parent.left
                right: parent.right
                top: icon.bottom
                bottom: parent.bottom
                leftMargin: 10
                rightMargin: 10
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            elide: Text.ElideRight
        }
    }

    MultiEffect {
        id: shadowEff
        source: componentRect
        anchors.fill: componentRect
        shadowEnabled: true
        shadowOpacity: 0.7
    }

    states: [
        State {
            name: statePressed
            when: mouseArea.pressed
            PropertyChanges {
                target: componentRect
                scale: 0.95
            }
        },
        State {
            name: stateRealesed
            when: !mouseArea.pressed
            PropertyChanges {
                target: componentRect
                scale: 1.0
            }
        }
    ]

    transitions: [
        Transition {
            to: statePressed
            ParallelAnimation {
                PropertyAnimation {
                    target: componentRect
                    property: "scale"
                    from: 1.0
                    to: 0.95
                    duration: 50
                }
                PropertyAnimation {
                    target: shadowEff
                    property: "scale"
                    from: 1.0
                    to: 0.95
                    duration: 50
                }
            }
        },
        Transition {
            to: stateRealesed
            ParallelAnimation {
                PropertyAnimation {
                    target: componentRect
                    property: "scale"
                    from: 0.95
                    to: 1.0
                    duration: 100
                }
                PropertyAnimation {
                    target: shadowEff
                    property: "scale"
                    from: 0.95
                    to: 1.0
                    duration: 100
                }
            }
        }
    ]

//    Component.onCompleted: {
//        console.log("EvAct construct")
//    }

//    Component.onDestruction: {
//        console.log("EvAct destruction")
//    }
}
