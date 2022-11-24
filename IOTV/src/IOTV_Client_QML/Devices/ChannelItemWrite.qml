import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias text: txtField.text
    property int number: 0
    property alias type: typeName.text
    property alias button: btn

    required property var _device

    height: 50

    Label {
        id: lb1
        text: number + ":"
        font.pixelSize: 12
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: 10
            rightMargin: 10
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Label {
        id: typeName
        text: "type"
        font.pixelSize: 12
        anchors {
            left: lb1.right
            top: parent.top
            bottom: parent.bottom
            leftMargin: 10
            rightMargin: 10
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    TextField {
        id: txtField
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        font.pixelSize: 12
        enabled: device.state

        text: ""
        placeholderText: "Введите данные"
        placeholderTextColor: "#ccc"
        anchors {
            left: typeName.right
            top: parent.top
            right: btn.left
            bottom: parent.bottom
            leftMargin: 10
            rightMargin: 10
        }
    }
    Button {
        id: btn

        width: 46
        height: 46

        font.pixelSize: 12
        icon {
            color: "transparent"
            source: "qrc:/img/send.png"
        }

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 10
        }

        onClicked: {
            _device.setDataFromString(number, txtField.text)
        }
    }
}
