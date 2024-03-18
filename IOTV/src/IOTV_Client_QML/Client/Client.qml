import QtQuick 2.9
import QtQuick.Controls 2.5
import Qt.labs.settings 1.1

Page {
    id: root
    title: "Настройки подключения"

    readonly property string stateConnected: "Connected"
    readonly property string stateConnecting: "Connecting"
    readonly property string stateDisconnected: "Disconnected"

    property alias btn: btn_connect

    state: stateDisconnected

    Flickable {
        width: parent.width
        height: parent.height
        contentHeight: 200

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }
        Column
        {
            width: parent.width
            spacing: 15
            padding: 30

            TextField {
                id: addr
                height: 52
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 30
                    rightMargin: 30
                }

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                antialiasing: true
                font.pixelSize: 14
                placeholderText: "Адрес сервера"
                placeholderTextColor: "#cccccc"

                text: settings.address
                onTextChanged: settings.address = text
            }

            TextField {
                id: port
                height: 52
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 30
                    rightMargin: 30
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                layer.textureMirroring: ShaderEffectSource.MirrorVertically
                antialiasing: true
                font.pixelSize: 14
                placeholderText: "Порт сервера"
                placeholderTextColor: "#cccccc"

                validator: IntValidator {
                    bottom: 0
                }

                text: settings.port

                onTextChanged: settings.port = text
            }
            CheckBox {
                id: autoConnect
                height: 52
                text: qsTr("Подключение при запуске программы")
                font.pixelSize: 14
                antialiasing: true

                checked: settings.autoConnect
            }

            Button {
                id: btn_connect
                width: 180
                height: 60
                font.pixelSize: 18
                anchors.horizontalCenter: parent.horizontalCenter
                highlighted: true

                onClicked: {
                    if (!addr.text.length) {
                        addr.focus = true
                        return
                    }
                    if (!port.text.length) {
                        port.focus = true
                        return
                    }

                    if (!client.state)
                        client.connectToHost(addr.text, port.text)
                    else
                        client.disconnectFromHost()
                }
            }
        }
    }

    states: [
        State {
            name: stateConnected
            PropertyChanges {
                target: addr
                enabled: false
            }
            PropertyChanges {
                target: port
                enabled: false
            }
            PropertyChanges {
                target: autoConnect
                enabled: false
            }
            PropertyChanges {
                target: btn_connect
                text: "отключиться"
            }
            PropertyChanges {
                target: loaderMainItem
                source: ""
            }
        },
        State {
            name: stateConnecting
            PropertyChanges {
                target: btn_connect
                text: "подключение..."
            }
        },
        State {
            name: stateDisconnected
            PropertyChanges {
                target: addr
                enabled: true
            }
            PropertyChanges {
                target: port
                enabled: true
            }
            PropertyChanges {
                target: autoConnect
                enabled: true
            }
            PropertyChanges {
                target: btn_connect
                text: "подключиться"
            }
            PropertyChanges {
                target: loaderMainItem
                source: ""
            }
        }
    ]

    Settings {
        id: settings
        category: "Client"
        property alias address: addr.text
        property alias port: port.text
        property alias autoConnect: autoConnect.checked
    }

    Connections {
        target: client
        function onSignalConnected() {
            state = stateConnected
            glob_notification.set_text("cоединение установлено")
        }
        function onSignalConnecting() {
            state = stateConnecting
        }
        function onSignalDisconnected() {
            state = stateDisconnected
            glob_notification.set_text("cоединение сброшено")
            appStack.clear()
            appStack.push(homePage)
        }
    }

    Component.onCompleted: {
        console.log("Client page construct: ", objectName)
        if (settings.autoConnect)
            client.connectToHost(addr.text, port.text)
    }

    Component.onDestruction: {
        console.log("Client page destruct: ", objectName)
    }

    // вызывается из Home.qml
    function connectToHost() {
        btn_connect.clicked()
    }
}
