import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Effects

Item {
    readonly property string statePressed: "pressed"
    readonly property string stateRealesed: "released"
    readonly property string stateChecked: "stateChecked"
    readonly property string stateUnChecked: "stateUnChecked"

    property bool checkable: false
    property bool checked: false

    property alias ledVisible: led.visible
    property alias ledEnable: led.ledEnable

    property alias label: lbl
    property alias icon: img

    signal signalClicked()
    signal signalToggled()

    id: root

    width: global_window.width / 2 - global_window.width * 0.1
    height: 110

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked:
        {
            if (checkable)
            {
                checked = !checked
                signalToggled()
            }
            else
                signalClicked()
        }
    }

    Rectangle {
        id: componentRect
        anchors.centerIn: parent

        width: root.width - 15
        height: root.height - root.height * 0.1

        color: Qt.rgba(255, 255, 255, 1)
        radius: 5

        Image {
            id: img
            //            source: model.source
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
            id: lbl
            text: "ButtonShadow"
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

        Led_Indicator {
            id: led
            visible: false
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: 10
                topMargin: 10
            }
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
            when: mouseArea.pressed && !checkable
        },
        State {
            name: stateRealesed
            when: !mouseArea.pressed && !checkable
        },
        State {
            name: stateChecked
            when: checked && checkable
            PropertyChanges {
                target: componentRect
                color: "#3f51b5"
            }
            PropertyChanges {
                target: lbl
                color: "white"
            }
        },
        State {
            name: stateUnChecked
            when: !checked && checkable
            PropertyChanges {
                target: componentRect
                color: Qt.rgba(255, 255, 255, 1)
            }
            PropertyChanges {
                target: lbl
                color: "black"
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
        },
        Transition {
            to: stateChecked
            ParallelAnimation {
                PropertyAnimation {
                    target: componentRect
                    property: "scale"
                    from: 1.0
                    to: 0.95
                    duration: 50
                }
                ColorAnimation {
                    target: componentRect
                    duration: 200
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
            to: stateUnChecked
            ParallelAnimation {
                PropertyAnimation {
                    target: componentRect
                    property: "scale"
                    from: 0.95
                    to: 1.0
                    duration: 100
                }
                ColorAnimation {
                    target: componentRect
                    duration: 200
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
}
