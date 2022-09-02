import QtQuick 2.9
import QtQuick.Controls 2.2
//import io.qt.Backend_Client 1.0

Page {
    id: root
    title: "Server"
//    anchors.fill: parent

    property bool connection_attempt: false

    Item {
        anchors.fill: parent
        anchors.centerIn: parent.verticalCenter

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
            anchors.top: parent.top
            anchors.left: label1.right
            anchors.right: parent.right
            anchors.margins: 5
            enabled: !indicator.visible
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
            enabled: !indicator.visible
            anchors.right: parent.right
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: 5
            text: client.port

            onTextChanged: client.port = text
        }

        Button {
            id: btn
            font.pixelSize: 18
            enabled: !indicator.visible
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: parent.horizontalCenter
            hoverEnabled: true
            autoRepeat: false
            checkable: false
            display: AbstractButton.TextBesideIcon
            highlighted: false
            flat: false
            text: "Подключиться"
            anchors.top: autoConnect.bottom

            onClicked: {
                if(connection_attempt)
                {
                    client.disconnectFromHost()
                    btn.text = "Подключиться"
                    connection_attempt = false;
                    addr.readOnly = port.readOnly = false
                    autoConnect.enabled = true

                    addr.color = port.color = "white"
                }
                else if(!client.state)
                {
                    client.connectToHost()
                    btn.text = "Подключение..."
                    connection_attempt = true;
                    addr.readOnly = port.readOnly = true
                    autoConnect.enabled = false

                    addr.color = port.color = "gainsboro"
                }
                else
                    client.disconnectFromHost()
            }
        }

        CheckBox {
            id: autoConnect
            height: label2.height
            text: qsTr("Автоподключение")
            anchors.left: parent.left
            anchors.top: label2.bottom
            font.pixelSize: 18
            enabled: !indicator.visible
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
    }

    Rectangle {
        id: recIndicator
        anchors.fill: parent
        layer.enabled: false
        antialiasing: true
        color: "#aacccccc"
        focus: true
        visible: connection_attempt
        BusyIndicator {
            id: indicator
            antialiasing: true
            anchors.centerIn: parent
        }
    }


    Connections {
        target: client
        function onSignalConnected() {
            connection_attempt = false
            btn.text = "Отключиться"
            addr.readOnly = port.readOnly = true
            autoConnect.enabled = false
            addr.color = port.color = "gainsboro"
            recIndicator.visible = false

        }
        function onSignalDisconnected() {
            connection_attempt = false
            btn.text = "Подключиться"
            addr.readOnly = port.readOnly = false
            autoConnect.enabled = true
            addr.color = port.color = "white"
        }
        function onSignalConnectWait() {
            btn.text = "Подключиться"
            connection_attempt = false;
            addr.readOnly = port.readOnly = false
            autoConnect.enabled = true

            addr.color = port.color = "white"
        }
    }

    Component.onCompleted: {
        connection_attempt = client.autoConnect
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2}D{i:3}D{i:4}D{i:5}D{i:6}D{i:7}D{i:8}
D{i:1}D{i:10}D{i:9}
}
##^##*/
