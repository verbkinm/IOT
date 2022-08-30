import QtQuick
import QtQuick.Controls
//import io.qt.Backend_Server 1.0


Page {
    width: 600
    height: 400

    property bool status: false
    property bool connection_attempt: false

//    property alias back: backend_server

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
        id: flickable
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
