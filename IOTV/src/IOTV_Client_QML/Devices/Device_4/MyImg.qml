import QtQuick 2.9
import QtQuick.Controls 2.2

import "qrc:/Devices/" as Devices
import "qrc:/Devices/BaseItem" as BaseItem

Image {
    id: root
    height: 200
    width: 200

    fillMode: Image.PreserveAspectFit

    property string playing: "playing"
    property string stoped: "stoped"

    Image {
        id: note1
        source: "qrc:/img/id_4/note.png"
        fillMode: Image.PreserveAspectFit
        width: 42
        height: width
        opacity: 0

        property int startX: parent.width / 2 - width / 2
        property int startY: parent.height / 2 - height / 2

        x: startX
        y: startY
    }

    Image {
        id: note2
        source: "qrc:/img/id_4/note2.png"
        fillMode: Image.PreserveAspectFit
        width: 42
        height: width
        opacity: 0

        property int startX: parent.width / 2 - width / 2
        property int startY: parent.height / 2 - height / 2

        x: startX
        y: startY
    }

    Image {
        id: note3
        source: "qrc:/img/id_4/note.png"
        fillMode: Image.PreserveAspectFit
        width: 42
        height: width
        opacity: 0

        property int startX: parent.width / 2 - width / 2
        property int startY: parent.height / 2 - height / 2

        x: startX
        y: startY
    }

    states: [
        State {
            name: playing
        },
        State {
            name: stoped
            PropertyChanges {
                target: note1
                x: note1.startX
                y: note1.startY
            }
            PropertyChanges {
                target: note2
                x: note2.startX
                y: note2.startY
            }
            PropertyChanges {
                target: note3
                x: note3.startX
                y: note3.startY
            }
        }
    ]

    transitions: [
        Transition {
            to: playing
            ParallelAnimation {
                loops: Animation.Infinite

                SequentialAnimation {
                    PropertyAnimation {
                        target: note1
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 1500
                    }
                    PropertyAnimation {
                        target: note1
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 1500
                    }
                }

                PropertyAnimation {
                    target: note1
                    property: "scale"
                    easing.type: Easing.OutElastic
                    from: 0
                    to: 1
                    duration: 2000
                }
                NumberAnimation { target: note1; property: "x"; easing.type: Easing.OutInQuad; to: -50; duration: 3000 }
                NumberAnimation { target: note1; property: "y"; to: -50; duration: 3000 }
            }

            SequentialAnimation{
                loops: Animation.Infinite

                PropertyAnimation {
                    duration: 555
                }
                ParallelAnimation {
                    SequentialAnimation {
                        PropertyAnimation {
                            target: note2
                            property: "opacity"
                            from: 0
                            to: 1
                            duration: 1500
                        }
                        PropertyAnimation {
                            target: note2
                            property: "opacity"
                            from: 1
                            to: 0
                            duration: 1500
                        }
                    }

                    PropertyAnimation {
                        target: note2
                        property: "scale"
                        easing.type: Easing.OutElastic
                        from: 0
                        to: 1
                        duration: 2000
                    }
                    NumberAnimation { target: note2; property: "x"; easing.type: Easing.OutInQuad; to: 200; duration: 3000 }
                    NumberAnimation { target: note2; property: "y"; to: -50; duration: 3000 }
                }
            }

            SequentialAnimation{
                loops: Animation.Infinite

                PropertyAnimation {
                    duration: 1000
                }
                ParallelAnimation {
                    SequentialAnimation {
                        PropertyAnimation {
                            target: note3
                            property: "opacity"
                            from: 0
                            to: 1
                            duration: 1500
                        }
                        PropertyAnimation {
                            target: note3
                            property: "opacity"
                            from: 1
                            to: 0
                            duration: 1500
                        }
                    }

                    SequentialAnimation {
                        PropertyAnimation {
                            target: note3
                            property: "scale"
                            easing.type: Easing.OutElastic
                            from: 0
                            to: 1
                            duration: 1500
                        }
//                        PropertyAnimation {
//                            target: note3
//                            property: "scale"
//                            easing.type: Easing.OutElastic
//                            from: 0
//                            to: 1
//                            duration: 1500
//                        }
                    }
                    NumberAnimation { target: note3; property: "x"; duration: 3000 }
                    NumberAnimation { target: note3; property: "y"; to: -50; duration: 3000 }
                }
            }
        },
        Transition {
            to: stoped
            ParallelAnimation {
                PropertyAnimation {
                    target: note1
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 0
                }
                PropertyAnimation {
                    target: note2
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 0
                }
            }
        }
    ]

    Component.onCompleted:
    {
        var currState = state

        state = ""
        state = currState
    }
}
