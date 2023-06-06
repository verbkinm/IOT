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
//        else
//            listModel.clear()
    }

    Flickable {
        width: parent.width
        height: parent.height
        contentHeight: 200

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
                height: 45

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

            Text {
                text: qsTr("Тут будет список событий")
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 18
                wrapMode: Text.Wrap
            }

            EvActList {
                id: listView
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
