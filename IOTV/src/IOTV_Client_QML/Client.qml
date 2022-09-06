import QtQuick 2.9
import QtQuick.Controls 2.5

Page {
    id: root
    title: "Настройки подключения"

    property bool connection_attempt: false

    Item {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: childrenRect.height

        Label {
            id: label1
            width: 100
            height: addr.height
            text: "Адресс: "
            font.pixelSize: 18
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            anchors {
                left: parent.left
                top: parent.top
                margins: 10
            }
            leftPadding: 20
        }

        Label {
            id: label2
            width: label1.width
            height: label1.height
            text: "Порт: "
            font.pixelSize: 18
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            anchors {
                left: parent.left
                top: label1.bottom
                margins: 10
            }
            leftPadding: 20
        }

        TextField {
            id: addr
            height: 40
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            antialiasing: true
            font.pixelSize: 18
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
            text: client.addr
            onTextChanged: client.addr = text
        }

        TextField {
            id: port
            width: addr.width
            height: addr.height
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            antialiasing: true
            font.pixelSize: 18
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
            text: client.port

            onTextChanged: client.port = text
        }
        CheckBox {
            id: autoConnect
            height: label2.height
            text: qsTr("Автоподключение")
            anchors {
                left: parent.left
                top: label2.bottom
                margins: 10
            }
            leftPadding: 20

            font.pixelSize: 18

            enabled: !client.state
            checked: client.autoConnect

            onClicked: {
                client.autoConnect = checked
            }
        }

        Button {
            id: btn
            width: 160
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
                    client.connectToHost()
                    connection_attempt = true;
                }
                else
                    client.disconnectFromHost()
            }
        }
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
        }
    }

    Component.onCompleted: {
        connection_attempt = client.autoConnect
    }
}
