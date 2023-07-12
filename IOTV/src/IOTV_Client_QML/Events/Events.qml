import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/Devices/" as Devices

Page {
    id: root
    title: "Настройки событий"

    onVisibleChanged: {
        if (visible)
        {
            client.queryEventAction()
            timer.start()
        }
    }

    Flickable {
        id: flickable
        width: parent.width
        height: parent.height
//        contentHeight: listView.height + addEvent.height

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

            Button {
                id: addEvent
                width: parent.width * 0.8
                height: 60

                text: "Добавить событие"
                flat: false
                highlighted: true

                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                onClicked: {
                    var component = Qt.createComponent("qrc:/Events/AddEvent.qml");
                    if (component.status === Component.Ready)
                        appStack.push(component)
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
