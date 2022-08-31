import QtQuick 2.9
import QtQuick.Controls 2.5
import io.qt.Backend_Client 1.0

Page {
    property bool status: false
    property bool connection_attempt: false

    property int countDevice: 0
    property int countDeviceOnline: 0

    title: "Server"

    property alias back: backend_client

    signal click()
    signal disconected()

    Flickable
    {
        id: flickable_server
        anchors.topMargin: 15
        anchors.fill: parent
        contentHeight: childrenRect.height
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
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            text: "127.0.0.1"
            //        inputMask: "009.009.009.009"
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
            anchors.right: parent.right
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: 5
            text: "2022"
            //        inputMask: "99999"
        }
        Backend_Client {
            id: backend_client

            onSignalConnected: {
                connection_attempt = false
                status = true
                btn.text = "Отключиться"
                addr.readOnly = port.readOnly = true
                autoConnect.enabled = true

                addr.color = port.color = "gainsboro"
            }
            onSignalDisconnected: {
                connection_attempt = false
                status = false
                btn.text = "Подключиться"
                addr.readOnly = port.readOnly = false
                autoConnect.enabled = false

                addr.color = port.color = "white"

                disconected()
            }
            onAddrChanged: {
                addr.text = addr
            }
            onPortChanged: {
                port.text = port
            }
            onCountDeviceChanged: {
                countDevice = backend_client.totalDevice
            }
            onOnlineDeviceChanged: {
                countDeviceOnline = backend_client.onlineDevice
            }
        }

        Button {
            id: btn
            font.pixelSize: 18
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: parent.horizontalCenter
            hoverEnabled: true
            autoRepeat: false
            checkable: false
            display: AbstractButton.TextBesideIcon
            highlighted: false
            flat: false
            topPadding: 12
            text: "Подключиться"
            anchors.top: autoConnect.bottom

            onClicked: {
                if(connection_attempt)
                {
                    backend_client.disconnectFromHost()
                    btn.text = "Подключиться"
                    connection_attempt = false;
                    addr.readOnly = false
                    port.readOnly = false
                    autoConnect.enabled = true

                    addr.color = "white"
                    port.color = "white"
                }
                else if(!status)
                {
                    backend_client.addr = addr.text
                    backend_client.port = port.text
                    backend_client.connectToHost()
                    btn.text = "Подключение..."
                    connection_attempt = true;
                    addr.readOnly = true
                    port.readOnly = true
                    autoConnect.enabled = false

                    addr.color = "gainsboro"
                    port.color = "gainsboro"
                }
                else
                    backend_client.disconnectFromHost()
            }
        }

        CheckBox {
            id: autoConnect
            height: label2.height
            text: qsTr("Автоподключение")
            anchors.left: parent.left
            anchors.top: label2.bottom
            font.pixelSize: 18
            anchors.leftMargin: 2
            layer.smooth: false
            anchors.topMargin: 5
            font.hintingPreference: Font.PreferFullHinting
            display: AbstractButton.TextOnly
            autoRepeat: false
            autoExclusive: false
            checked: false
        }
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.9}D{i:2}D{i:3}D{i:4}D{i:5}D{i:6}D{i:7}D{i:1}
}
##^##*/
