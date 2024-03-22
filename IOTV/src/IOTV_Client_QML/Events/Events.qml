import QtQuick 2.15
import QtQuick.Controls 2.5

import "qrc:/Devices/" as Devices

Page {
    id: root
    title: "Настройки событий"

    RoundButton {
        id: addEvent
        width: 64
        height: 64
        radius: width / 2

        text: "+"
        highlighted: true
        font.pixelSize: 36

        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 15
        }
        z: 1

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

        anchors {
            leftMargin: parent.width * 0.1
            rightMargin: parent.width * 0.1
            topMargin: 15
            bottomMargin: 25

//            top: addEvent.bottom
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    Devices.BusyRect {
        id: popup
        anchors.fill: parent
        visible: timer.running
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

    function destroyEv()
    {
        listView.destroyEv()
    }
}
