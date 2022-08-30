import QtQuick
import QtQuick.Controls
//import io.qt.Backend_Server 1.0

Page {
    width: 600
    height: 400

    property bool status: false
    property bool connection_attempt: false

    property alias back: backend_server

    signal click()
    signal disconected()

//    header:
//        ToolBar {
//            Label {
//            anchors.centerIn: parent
//            text: qsTr("Сервер: " + textHeader())
//            font.pixelSize: Qt.application.font.pixelSize * 2

//            function textHeader()
//            {
//                return (status ? "<font color='chartreuse'>подключен</font>" : "<font color='red'>отключен</font>")
//            }
//        }
//    }

    Flickable
    {
        anchors.topMargin: 15
        anchors.fill: parent
        contentHeight: childrenRect.height
        Label {
            id: label1
            text: "Адресс: "
            font.pixelSize: 24
            anchors.left: parent.left
            anchors.leftMargin: 5
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
            text: "Порт: "
            font.pixelSize: 24
            anchors.left: parent.left
            anchors.leftMargin: 5
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

        Label {
            id: label3
            text: "Кол-во устройств: "
            font.pixelSize: 24
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.top: label2.bottom
        }
        Label {
            id: devices
            text: "(<font color='green'>0</font>/<font color='red'>0</font>)"
            font.pixelSize: 24

            anchors.left: label3.right
            anchors.leftMargin: 5
            anchors.top: label2.bottom
        }

//        Backend_Server {
//            id: backend_server

//            onSignalConnected: {
//                connection_attempt = false
//                status = true
//                btn.text = "Отключиться"
//                addr.readOnly = true
//                port.readOnly = true

//                addr.color = "gainsboro"
//                port.color = "gainsboro"
//            }
//            onSignalDisconnected: {
//                connection_attempt = false
//                status = false
//                btn.text = "Подключиться"
//                addr.readOnly = false
//                port.readOnly = false

//                addr.color = "white"
//                port.color = "white"

//                devices.text = "(<font color='green'>0</font>/<font color='red'>0</font>)"
//                disconected()
//            }
//            onAddrChanged: {
//                addr.text = addr
//            }
//            onPortChanged: {
//                port.text = port
//            }
//            onTotalDeviceChanged: {
//                devices.text = "(<font color='green'>" + onlineDevice + "</font>/<font color='red'>" + (totalDevice - onlineDevice) + "</font>)"
//            }
//        }

        Button {
            id: btn
            anchors.top: devices.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 15
            font.pixelSize: 24
            text: "Подключиться"

//            onClicked: {
//                if(connection_attempt)
//                {
//                    backend_server.disconnectFromHost()
//                }
//                else if(!status)
//                {
//                    backend_server.addr = addr.text
//                    backend_server.port = port.text
//                    backend_server.connectToHost()
//                    btn.text = "Подключение..."
//                    connection_attempt = true;
//                    addr.readOnly = true
//                    port.readOnly = true

//                    addr.color = "gainsboro"
//                    port.color = "gainsboro"
//                }
//                else
//                {
//                    backend_server.disconnectFromHost()
//                }
//            }
        }
    }
}
