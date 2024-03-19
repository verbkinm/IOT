import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/Devices/" as Devices

Page {
    id: root
    title: "Настройки событий"

    //        if (visible)
    //        {
    //            client.queryEventAction()
    //            timer.start()
    //            popup.open()
    //        }
    //    }

    Flickable {
        id: flickable
        width: parent.width
        height: parent.height

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Column {
            id: column
            width: parent.width
            anchors {
                top: parent.top
                topMargin: 20
            }
            spacing: 15

            Button {
                id: addEvent
                width: parent.width * 0.8
                height: 60

                text: "Добавить событие"
                highlighted: true
                font.pixelSize: 18

                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                onClicked: {
                    var component = Qt.createComponent("qrc:/Events/AddEvent.qml");
                    if (component.status === Component.Ready)
                    {
                        var obj = component.createObject(null, {btnDeleteVisible: false})
                        glob_eventStackView.push(obj)
                    }
                }
            }

            EvActList {
                id: listView

                onContentHeightChanged: {
                    flickable.contentHeight = listView.contentHeight + addEvent.height + 45
                }
            }
        }
    }

    Devices.BusyRect {
        id: popup
        anchors.fill: parent
        visible: true
    }

    Component.onCompleted: {
        console.log("Events page construct: ", objectName)
        client.queryEventAction()
        timer.start()
    }

    Component.onDestruction: {
        console.log("Events page destruct: ", objectName)
    }

    Timer {
        id: timer
        interval: 2000
        repeat: true
        running: false
        onTriggered: {
            client.queryEventAction()
        }
    }
}
