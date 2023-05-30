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

            onClicked: {
//                loaderDevice.setSource(createDeviceBy(client.deviceByName(model.name).id), {device: client.deviceByName(model.name)})
//                loaderDevice.title = loaderDevice.objectName = Qt.binding( function() {return  client.deviceByName(model.name).aliasName})
                //!!!
                loaderNewAction.source = "qml:/Events/AddEvent.qml"
                console.log(loaderNewAction.title)
                loaderNewAction.title = loaderNewAction.objectName
                appStack.push(loaderNewAction)
            }
        }
    }


//    states: [
//        State {
//            name: stateConnected
//            PropertyChanges { target: addr; enabled: false}
//            PropertyChanges { target: port; enabled: false}
//            PropertyChanges { target: autoConnect; enabled: false}
//            PropertyChanges { target: btn_connect; text: "отключиться"}
//            PropertyChanges { target: loaderMainItem; source: ""}
//        },
//        State {
//            name: stateConnecting
//            PropertyChanges { target: btn_connect; text: "подключение..."}
//        },
//        State {
//            name: stateDisconnected
//            PropertyChanges { target: addr; enabled: true}
//            PropertyChanges { target: port; enabled: true}
//            PropertyChanges { target: autoConnect; enabled: true}
//            PropertyChanges { target: btn_connect; text: "подключиться"}
//            PropertyChanges { target: loaderMainItem; source: ""}
//        }
//    ]

//    Connections {
//        target: client
//        function onSignalConnected() {
//            state = stateConnected
//            loaderMainItem.setSource("qrc:/Notification.qml", {parent: appStack, text: "cоединение установлено"})
//        }
//        function onSignalConnecting() {
//            state = stateConnecting
//            loaderMainItem.setSource("qrc:/PopupWait.qml", {parent: appStack})
//        }
//        function onSignalDisconnected() {
//            state = stateDisconnected
//            loaderMainItem.setSource("qrc:/Notification.qml", {parent: appStack, text: "cоединение сброшено"})
//        }
//    }

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
    }
}
