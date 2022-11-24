import QtQuick 2.9
import QtQuick.Controls 2.5
import Qt.labs.settings 1.1

Page {
    id: root
    title: "Настройки подключения"

    property bool connection_attempt: false

    Flickable {
        width: parent.width
        height: parent.height
        contentHeight: 200

        ScrollBar.vertical: ScrollBar {
            id: scroll
            visible: active
        }

        GroupBox {
            title: "Подключение к серверу"
            font.pixelSize: 14
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
                enabled: !client.state
                text: settings.address
                onTextChanged: settings.address = text
            }

            TextField {
                id: port
                width: addr.width
                height: addr.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                antialiasing: true
                font.pixelSize: 14
                placeholderText: "Введите порт сервера"
                placeholderTextColor: "#cccccc"

                enabled: !client.state
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

                enabled: !client.state
                checked: settings.autoConnect

//                onClicked: {
//                    client.autoConnect = checked
//                }
            }

            Button {
                id: btn
                width: 180
                font.pixelSize: 18
                anchors.horizontalCenter: parent.horizontalCenter
                text: {
                    if (client.state)
                        return "отключиться"
                    else if (connection_attempt)
                        return "подключение..."
                    else
                        return "подключиться"
                }
                anchors.top: autoConnect.bottom
                anchors.margins: 10

                onClicked: {
                    if(connection_attempt)
                    {
                        client.disconnectFromHost()
                        connection_attempt = false;
                        autoConnect.enabled = true
                    }
                    else if(!client.state)
                    {
                        client.connectToHost(addr.text, port.text)
                        connection_attempt = true;
                    }
                    else
                        client.disconnectFromHost()
                }
            }
        }
    }

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
            connection_attempt = false
        }
        function onSignalDisconnected() {
            connection_attempt = false
        }
        function onSignalConnectWait() {
            connection_attempt = false;
            connectionError.open()
        }
    }

    Component.onCompleted: {
        connection_attempt = settings.autoConnect
        if (connection_attempt)
            client.connectToHost(addr.text, port.text)
    }

    Component.onDestruction: {
        console.log("Client page destruct: ", objectName)
    }
}
