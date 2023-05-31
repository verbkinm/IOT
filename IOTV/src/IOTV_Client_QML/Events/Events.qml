import QtQuick 2.9
import QtQuick.Controls 2.5

Page {
    id: root
    title: "Настройки событий"

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

            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 20
            }

            onReleased: {
                loaderNewAction.source = "qrc:/Events/AddEvent.qml"
                loaderNewAction.title = loaderNewAction.item.title
                appStack.push(loaderNewAction)
            }
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
}
