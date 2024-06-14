import QtQuick 2.9
import QtQuick.Controls 2.5

Item {
    property alias text: dataTextField.text
    property alias label: dataText.text
    property alias labelVisible: dataText.visible
    property alias placeholderText: dataTextField.placeholderText

    signal signalTextEdited();

    height: 50
    width: 400

    Text {
        id: dataText
        width: parent.width / 2
        text: "Данные:"

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20
        }
    }

    TextField {
        id: dataTextField
        focus: true
        width: parent.width / 2
        height: 52
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        font.pixelSize: 14
        text: ""
        placeholderText: "Введите данные..."
        placeholderTextColor: "#ccc"
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }

        onTextEdited: {
            signalTextEdited()
        }
    }
}
