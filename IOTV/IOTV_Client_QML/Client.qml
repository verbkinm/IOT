import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    id: root
    title: "Настройки подключения"
    anchors.fill: parent

    property bool connection_attempt: false


    Item {
        anchors.fill: parent
//        anchors.centerIn: parent.verticalCenter
        width: 324
        height: 150

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        Item {
            anchors.fill: parent
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: label1
                width: 89
                height: addr.height
                text: "Адресс: "
                font.pixelSize: 18
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: 10
                anchors.left: parent.left
                anchors.top: parent.top
            }

            LineEdit {
                id: addr
                height: 30;
                width: 120
                anchors.top: parent.top
                anchors.left: label1.right
                anchors.right: parent.right
                active: !client.state
                anchors.margins: 5
                text: client.addr

                onTextChanged: client.addr = text
            }

            Label {
                id: label2
                width: label1.width
                height: label1.height
                text: "Порт: "
                anchors.left: parent.left
                font.pixelSize: 18
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: 10
                anchors.topMargin: 5
                anchors.top: label1.bottom
            }

            LineEdit {
                id: port
                width: addr.width
                height: 30;
                anchors.top: addr.bottom
                active: !client.state
                anchors.right: parent.right
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                anchors.topMargin: 5
                text: client.port

                onTextChanged: client.port = text
            }
            CheckBox {
                id: autoConnect
                height: label2.height
                text: qsTr("Автоподключение")
                anchors.left: parent.left
                anchors.top: label2.bottom
                font.pixelSize: 18
                enabled: !client.state
                anchors.leftMargin: 10
                layer.smooth: false
                anchors.topMargin: 5
                font.hintingPreference: Font.PreferFullHinting
                display: AbstractButton.TextOnly
                autoRepeat: false
                autoExclusive: false
                checked: client.autoConnect

                onClicked: {
                    client.autoConnect = checked
                }
            }

            Button {
                id: btn
                width: 160
                font.pixelSize: 18
                autoExclusive: false
                anchors.horizontalCenter: parent.horizontalCenter
                hoverEnabled: true
                autoRepeat: false
                checkable: false
                text: {
                    if (client.state)
                        return "отключиться"
                    else if (connection_attempt)
                        return "подключение..."
                    else
                        return "подключиться"
                }
                anchors.top: autoConnect.bottom

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
                        autoConnect.enabled = false
                    }
                    else
                        client.disconnectFromHost()
                }
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
