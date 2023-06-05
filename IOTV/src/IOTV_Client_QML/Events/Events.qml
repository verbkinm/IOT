import QtQuick 2.9
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
        width: parent.width
        height: parent.height
        contentHeight: 200

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        Text {
            text: qsTr("Тут будет список событий")
            anchors.centerIn: parent
            font.pixelSize: 18
            wrapMode: Text.Wrap
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
                top: parent.top
                topMargin: 20
            }

            onReleased: {
                loaderNewAction.source = "qrc:/Events/AddEvent.qml"
                loaderNewAction.title = loaderNewAction.item.title
                loaderNewAction.objectName = loaderNewAction.item.title
                appStack.push(loaderNewAction)
            }
        }
    }

    Devices.BusyRect {
        id: popup
        anchors.fill: parent
        visible: true
    }

    Connections {
        target: client
        function onSignalEventAction() {
            popup.close()
            timer.stop()

        }
    }

    Component.onCompleted: {
        console.log("Events page construct: ", objectName)
    }

    Component.onDestruction: {
        console.log("Events page destruct: ", objectName)
    }

    // Добавление нового события
    Loader {
        property string title
        id: loaderNewAction
        source: ""

        onSourceChanged: {
            console.log ("change source")
        }
    }

    Timer {
        id: timer
        interval: 5000
        repeat: true
        running: false
        onTriggered: {
            client.queryEventAction()
        }
    }
}
