import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias text: txtField.text
    property alias number: lb1.text
    property alias type: typeName.text
    property alias button: btn

    signal signalWrite(string str)

    Label {
        id: lb1
        text: "1"
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
//                    readOnly: true

        text: ""
        placeholderText: "Введите данные"
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

        width: 32
        height: 32

        font.pixelSize: 12

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            leftMargin: 10
            rightMargin: 10
        }

        onClicked: {
            signalWrite(txtField.text)
        }
    }
}
