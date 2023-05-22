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

        GroupBox {
            id: groupBox
            height: 255
            anchors
            {
                top: parent.top
                left: parent.left
                right: parent.right
                leftMargin: 10
                rightMargin: 10
                bottomMargin: 10
                topMargin: 10
            }

            Label {
                id: label1
                width: 100
                height: addr.height
                text: "Адресс: "
                font.pixelSize: 14
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                anchors {
                    left: parent.left
                    top: parent.top
                    margins: 10
                }
            }

            Label {
                id: label2
                width: label1.width
                height: label1.height
                text: "Порт: "
                font.pixelSize: 14
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                anchors {
                    left: parent.left
                    top: label1.bottom
                    margins: 10
                }
            }

            TextField {
                id: addr
                height: 40
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                antialiasing: true
                font.pixelSize: 14
                placeholderText: "Введите адрес сервера"
                placeholderTextColor: "#cccccc"

                anchors {
                    left: label1.right
                    top: parent.top
                    right: parent.right
                    margins: 10
                    rightMargin: 20
                }
                text: settings.address
                onTextChanged: settings.address = text
            }

            TextField {
                id: port
                width: addr.width
                height: addr.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                layer.textureMirroring: ShaderEffectSource.MirrorVertically
                antialiasing: true
                font.pixelSize: 14
                placeholderText: "Введите порт сервера"
                placeholderTextColor: "#cccccc"
                validator: IntValidator{bottom: 0}

                anchors{
                    top: addr.bottom
                    left: label2.right
                    right: parent.right
                    margins: 10
                    rightMargin: 20
                }
                text: settings.port

                onTextChanged: settings.port = text
            }
            CheckBox {
                id: autoConnect
                height: label2.height
                text: qsTr("Подключение при запуске программы")
                anchors {
                    left: parent.left
                    top: label2.bottom
                    margins: 10
                }
                font.pixelSize: 14

                checked: settings.autoConnect
            }

            Button {
                id: btn_connect
                width: 180
                font.pixelSize: 18
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: autoConnect.bottom
                anchors.margins: 10

                onClicked: {
                    if (!addr.text.length)
                    {
                        addr.focus = true
                        return
                    }
                    if (!port.text.length)
                    {
                        port.focus = true
                        return
                    }

                    if(!client.state)
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
            PropertyChanges { target: addr; enabled: false}
            PropertyChanges { target: port; enabled: false}
            PropertyChanges { target: autoConnect; enabled: false}
            PropertyChanges { target: btn_connect; text: "отключиться"}
            PropertyChanges { target: loaderMainItem; source: ""}
        },
        State {
            name: stateConnecting
            PropertyChanges { target: btn_connect; text: "подключение..."}
        },
        State {
            name: stateDisconnected
            PropertyChanges { target: addr; enabled: true}
            PropertyChanges { target: port; enabled: true}
            PropertyChanges { target: autoConnect; enabled: true}
            PropertyChanges { target: btn_connect; text: "подключиться"}
            PropertyChanges { target: loaderMainItem; source: ""}
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
            loaderMainItem.setSource("qrc:/Notification.qml", {parent: appStack, text: "cоединение установлено"})
        }
        function onSignalConnecting() {
            state = stateConnecting
            loaderMainItem.setSource("qrc:/PopupWait.qml", {parent: appStack})
        }
        function onSignalDisconnected() {
            state = stateDisconnected
            loaderMainItem.setSource("qrc:/Notification.qml", {parent: appStack, text: "cоединение сброшено"})
        }
    }

    Component.onCompleted: {
        console.log("Client page construct: ", objectName)
        //        connection_attempt = settings.autoConnect
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
